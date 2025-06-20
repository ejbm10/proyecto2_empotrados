#include "system.h"
#include "stdint.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "io.h"
#include "unistd.h"

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02

int minutos = 0;
int segundos = 0;
int actualizar_display = 0;

char metadata[256];
int new_song = 0;
int song_active = 0;

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

void vga_clear()
{
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, CLEAR_COMMAND);
}

void vga_enable()
{
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, ENABLE_COMMAND);
}

void vga_write_char(int x, int y, char c)
{
    IOWR_8DIRECT(CHAR_BUFFER_BASE, (y * 80 + x), c);
}

void message_to_vga(const char* msg) {
	// Clear
	vga_clear();
	delay();
	vga_enable();
	delay();

	// Escribir la oración en pantalla
	for (int i = 0; i < strlen(msg); i++)
	{
		vga_clear();
		delay();
		vga_enable();
		delay();
		vga_write_char(5 + i, 1, msg[i]);  // fila Y=10
	}
}


void delay()
{
    volatile int i;
    for (i = 0; i < 1000000; i++);
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
    *timer_status_ptr = 0; // Limpiar status

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

void receive_metadata() {
	int i = 0;
	char val = '0';
	while (val != '/') {
		val = *metadata_fifo;
		metadata[i++] = val;
	}
	metadata[i--] = '\0';
	alt_printf("%s\n", metadata);
}

void receive_audio_data() {
	int16_t sample16 = (int16_t) (*song_fifo & 0xFFFF);
	int32_t sample32 = sample16 << 8;

	if ((((*audio_fifospace >> 24) & 0xFF) > 0) && ((*audio_fifospace >> 16) & 0xFF) > 0) {
		*audio_leftdata = sample32;
		*audio_rightdata = sample32;
	}

	usleep(0);
}
// Main
int main() {
	alt_putstr("Hello from Nios\n");

    *buttons_edge_ptr = 0;
    *buttons_mask_ptr = 0xF; // Habilitar interrupciones botones 0-3

    alt_irq_register(REG_BUTTONS_IRQ, NULL, button_isr_handler);

    *timer_periodl_ptr = TIMER_PERIOD & 0xFFFF;
    *timer_periodh_ptr = (TIMER_PERIOD >> 16) & 0xFFFF;

    *timer_control_ptr = 0x7; // START + CONT + ITO

    alt_irq_register(TIMER_IRQ, NULL, timer_isr_handler);

	*config_control = 0x1;	// Reset 1 for auto-initializing core
	*config_control = 0x0;	// Reset 0 for normal flow

	while (((*config_status >> 8) & 0x1) == 0 &&
			((*config_status >> 1) & 0x1) == 0);	// Wait auto-initializing

	*audio_control = 0xC;	// Set clears to 1
	*audio_control = 0x0;	// Set clears to 0 for normal	 flow

	new_song = 1;

    while (1) {
    	if (new_song) {
    		receive_metadata();
    		song_active = 1;
    		new_song = 0;
    	}

    	if (song_active) {
    		receive_audio_data();
    	}

    	if (actualizar_display) {
    		mostrar_duracion(minutos, segundos);
    		actualizar_display = 0;
    	}
    }
}


