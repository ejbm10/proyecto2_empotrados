#include "system.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_avalon_pio_regs.h"
#include "io.h"

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02

int minutos = 0;
int segundos = 0;
int paused = 0;
int actualizar_display = 0;

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

void wait() {
	while ((*config_status & 0x102) == 0);
}

void config_wm8731(alt_u8 addr, alt_u16 data) {
	wait();

	*config_address = addr;
	*config_data = data;

	*config_control = 0x340002;
}

void init_wm8731() {
	config_wm8731(0x0F, 0x000); // Reset CODEC

	wait();

	config_wm8731(0x00, 0x097); // Left Line In default
	config_wm8731(0x01, 0x097); // Right Line In default
	config_wm8731(0x02, 0x07F);	// Left output full volume
	config_wm8731(0x03, 0x07F);	// Right output full volume
	config_wm8731(0x04, 0x012);	// Analog Audio Config: Using DAC, Line In
	config_wm8731(0x05, 0x000); // Digital Audio Config: Output unmuted, no filter
	config_wm8731(0x06, 0x047); // Power off inputs and clock output (not needed)
	config_wm8731(0x07, 0x009); // A bunch of config
	config_wm8731(0x08, 0x000); // Sampling rate 48kHz normal
	config_wm8731(0x09, 0x001); // Activate
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

    if (buttons == 0x8) paused = !paused;
    else if (buttons == 0x2) {
    	minutos = 0;
    	segundos = 0;
    } else if (buttons == 0x1) {
    	minutos = 0;
    	segundos = 0;
    }
}


// ISR timer
void timer_isr_handler(void* context, alt_u32 id) {
    *timer_status_ptr = 0; // Limpiar status

    if (!paused) {
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

// Main
int main() {
    *buttons_edge_ptr = 0;
    *buttons_mask_ptr = 0xF; // Habilitar interrupciones botones 0-3

    alt_irq_register(REG_BUTTONS_IRQ, NULL, button_isr_handler);

    *timer_periodl_ptr = TIMER_PERIOD & 0xFFFF;
    *timer_periodh_ptr = (TIMER_PERIOD >> 16) & 0xFFFF;

    *timer_control_ptr = 0x7; // START + CONT + ITO

    alt_irq_register(TIMER_IRQ, NULL, timer_isr_handler);

	*config_control = 0x340003;
	*config_control = 0x340002;

	init_wm8731();

	*audio_control = 0xE;	// Set clears to 1
	*audio_control = 0x2;	// Set clears to 0 for normal flow

    while (1) {
        if (actualizar_display) {
            mostrar_duracion(minutos, segundos);
            actualizar_display = 0;
        }
        if ((*audio_control & 0x200) != 0) {

        }
    }
}


