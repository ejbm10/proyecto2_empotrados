#include "system.h"
#include "stdint.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "io.h"
#include "unistd.h"
#include "string.h"
#include <stdio.h>
#include "sys/alt_irq.h"

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02
#define MAX_COLS          60
#define MAX_ROWS          80
#define FIFO_2_CSR_BASE 0x00003020

int minutos = 0;
int segundos = 0;
int actualizar_display = 0;

char metadata[256];
int new_song = 0;
int song_active = 0;
int totalSamples = 0;

volatile unsigned int* segments_ptr = (unsigned int *) REG_SEGMENTS_BASE;

// Buttons
volatile unsigned int* buttons_edge_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x0C);
volatile unsigned int* buttons_mask_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x08);
volatile unsigned int* buttons_data_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x00);

// Timer
volatile unsigned int* timer_status_ptr = (unsigned int *) (TIMER_BASE + 0x00);
volatile unsigned int* timer_control_ptr = (unsigned int *) (TIMER_BASE + 0x04);
volatile unsigned int* timer_periodl_ptr = (unsigned int *) (TIMER_BASE + 0x08);
volatile unsigned int* timer_periodh_ptr = (unsigned int *) (TIMER_BASE + 0x0C);

// Audio
volatile unsigned int* audio_control = (unsigned int *) AUDIO_BASE;
volatile unsigned int* audio_fifospace = (unsigned int *) (AUDIO_BASE + 0x04);
volatile unsigned int* audio_leftdata = (unsigned int *) (AUDIO_BASE + 0x08);
volatile unsigned int* audio_rightdata = (unsigned int *) (AUDIO_BASE + 0X0C);

// Audio Config
volatile unsigned int* config_control = (unsigned int *) AUDIO_CONFIG_BASE;
volatile unsigned int* config_status = (unsigned int *) (AUDIO_CONFIG_BASE + 0x04);
volatile unsigned int* config_address = (unsigned int *) (AUDIO_CONFIG_BASE + 0x08);
volatile unsigned int* config_data = (unsigned int *) (AUDIO_CONFIG_BASE + 0x0C);

volatile unsigned int* song_fifo = (unsigned int *) FIFO_0_BASE;
volatile unsigned int* buttons_fifo = (unsigned int *) FIFO_1_IN_BASE;
volatile unsigned int* metadata_fifo = (unsigned int *) FIFO_2_BASE;
volatile unsigned int* metadata_fifo_csr = (unsigned int *) FIFO_2_CSR_BASE;

void vga_clear() {
    for (int y = 0; y < MAX_ROWS; y++) {
        for (int x = 0; x < MAX_COLS; x++) {
            IOWR_8DIRECT(CHAR_BUFFER_BASE, (y * MAX_COLS + x), ' ');
        }
    }
}

void vga_enable() {
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, ENABLE_COMMAND);
}

// Escribe una cadena en la posición X, Y (no borra toda la fila)
void vga_write_string(int x, int y, const char *text) {
    int len = strlen(text);
    if (x < 0) x = 0;
    if (x >= MAX_COLS) x = MAX_COLS - 1;
    if (y < 0) y = 0;
    if (y >= MAX_ROWS) y = MAX_ROWS - 1;

    for (int i = 0; i < len && (x + i) < MAX_COLS; i++) {
        IOWR_8DIRECT(CHAR_BUFFER_BASE, (y * MAX_COLS + x + i), text[i]);
    }
    for (int i = x + len; i < MAX_COLS; i++) {
        IOWR_8DIRECT(CHAR_BUFFER_BASE, (y * MAX_COLS + i), ' ');
    }
}

// Separa campos de la metadata con "-" y elimina el "/" final
void extraer_campos(const char *info, char campos[][64], int *num_campos) {
    int idx = 0, pos = 0;
    int len = strlen(info);

    for (int i = 0; i < 6; i++) campos[i][0] = '\0';

    for (int i = 0; i < len && idx < 6; i++) {
        if (info[i] == '-' || info[i] == '/') {
            campos[idx][pos] = '\0';
            idx++;
            pos = 0;
            if (info[i] == '/') break; // fin
        } else if (pos < 63) {
            campos[idx][pos++] = info[i];
        }
    }
    *num_campos = idx;
    // Depuración: imprime los campos extraídos
    for (int i = 0; i < *num_campos; i++)
        alt_printf("extract: [%d] = '%s'\n", i, campos[i]);
}

// Despliega los campos desplazados (cada campo en distinto XY)
void update_info_desplazada(char campos[][64], int num_campos) {
    const int x_offsets[] = {8, 16, 24, 32, 40, 48};
    const int y_start = 2;
    const int y_step  = 2;

    for (int i = 0; i < num_campos && i < 6; i++) {
    	alt_printf("Campo[%d]: %s\n", i, campos[i]);
        vga_write_string(x_offsets[i], y_start + i * y_step, campos[i]);
    }
}

unsigned int segmentos(int digito) {
    switch (digito) {
    case 0: return 0x40;
    case 1: return 0x79;
    case 2: return 0x24;
    case 3: return 0x30;
    case 4: return 0x19;
    case 5: return 0x12;
    case 6: return 0x2;
    case 7: return 0x78;
    case 8: return 0x0;
    case 9: return 0x18;
    default: return 0x40;
    }
}

// ISR botones

void button_isr_handler(void* context, alt_u32 id) {
    unsigned int buttons = *buttons_edge_ptr;
    *buttons_edge_ptr = buttons;

    //*buttons_fifo = buttons;


    if (buttons == 0x8 || buttons == 0x4) song_active = !song_active;
}


// ISR timer
void timer_isr_handler(void* context, alt_u32 id) {
    *timer_status_ptr = 1; // Limpiar status
    alt_putstr("IRQ!\n");
    if (song_active) {
        segundos++;
        if (segundos >= 60) {
            segundos = 0;
            minutos++;
        }
        if (minutos >= 99) {
            minutos = 0;
        }
    }
    actualizar_display = 1;
}

void mostrar_duracion(int minutos, int segundos) {
    int min_dec = minutos / 10;
    int min_uni = minutos % 10;
    int seg_dec = segundos / 10;
    int seg_uni = segundos % 10;

    unsigned int display_value =
        (segmentos(min_dec) << 21) |
        (segmentos(min_uni) << 14) |
        (segmentos(seg_dec) << 7) |
        (segmentos(seg_uni));

    *segments_ptr = display_value;
}

void receive_metadata_and_update_vga() {
    int i = 0;
    char val = 0;
    memset(metadata, 0, sizeof(metadata));
    int max_len = 128;
    while (i < max_len) {
        while ((*metadata_fifo_csr) == 0);
        val = *metadata_fifo;
        metadata[i++] = val;
        if (val == '/') break;
    }
    metadata[i] = '\0';

    alt_printf("Meta completa: [%s]\n", metadata);

    // Procesar y mostrar en VGA
    char campos[6][64];
    int num_campos = 0;
    extraer_campos(metadata, campos, &num_campos);

    vga_clear();
    vga_enable();
    update_info_desplazada(campos, num_campos);
}

void receive_audio_data() {
	int16_t sample16 = (int16_t) (*song_fifo & 0xFFFF);
	int32_t sample32 = sample16 << 8;

	if ((((*audio_fifospace >> 24) & 0xFF) > 0) && ((*audio_fifospace >> 16) & 0xFF) > 0) {
		*audio_leftdata = sample32;
		*audio_rightdata = sample32;
	}
	totalSamples -= 2;

	if (totalSamples <= 0) {
		song_active = 0;
		new_song = 1;
		totalSamples = 0;
		alt_putstr("Fin de la cancion\n");
		return;
	}
	usleep(0);
}
// Main

int main() {
	alt_putstr("Hello from Nios\n");

    *buttons_edge_ptr = 0;
    *buttons_mask_ptr = 0xF; // Habilitar interrupciones botones 0-3

    alt_irq_register(REG_BUTTONS_IRQ, NULL, button_isr_handler);

    //*timer_periodl_ptr = TIMER_PERIOD & 0xFFFF;
    //*timer_periodh_ptr = (TIMER_PERIOD >> 16) & 0xFFFF;

    *timer_periodl_ptr = 50000000 & 0xFFFF;
    *timer_periodh_ptr = (50000000 >> 16) & 0xFFFF;

    *timer_control_ptr = 0x7; // START + CONT + ITO

    alt_irq_register(TIMER_IRQ, NULL, timer_isr_handler);

	*config_control = 0x1;	// Reset 1 for auto-initializing core
	*config_control = 0x0;	// Reset 0 for normal flow

	while (((*config_status >> 8) & 0x1) == 0 &&
			((*config_status >> 1) & 0x1) == 0);	// Wait auto-initializing

	*audio_control = 0xC;	// Set clears to 1
	*audio_control = 0x0;	// Set clears to 0 for normal	 flow

	new_song = 1;
	totalSamples = 0;

	vga_clear();
	vga_enable();

    while (1) {
    	if (new_song) {
    		receive_metadata_and_update_vga();
    		song_active = 1;
    		new_song = 0;
    		minutos = 0;
    		segundos = 0;
    	}

    	if (song_active) {
    		if (totalSamples == 0) totalSamples = *song_fifo;
    		receive_audio_data();
    	}

    	if (actualizar_display) {
    	    // Deshabilita IRQs mientras accedes a variables compartidas
    	    alt_irq_context cpu_sr = alt_irq_disable_all();
    	    int min_copy = minutos;
    	    int seg_copy = segundos;
    	    actualizar_display = 0;
    	    alt_irq_enable_all(cpu_sr);

    	    mostrar_duracion(min_copy, seg_copy);
    	}
    }
}


