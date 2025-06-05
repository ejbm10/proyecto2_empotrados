#include "system.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"

#define TIMER_PERIOD 50000000 // 1 segundo @ 50 MHz

volatile int minutos = 0;
volatile int segundos = 0;
volatile int actualizar_display = 0;

volatile unsigned int* segments_ptr = (unsigned int *) REG_SEGMENTS_BASE;
volatile unsigned int* leds_ptr = (unsigned int *) LEDS_BASE;


volatile unsigned int* buttons_edge_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x0C);
volatile unsigned int* buttons_mask_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x08);
volatile unsigned int* buttons_data_ptr = (unsigned int *) (REG_BUTTONS_BASE + 0x00);

// Timer
volatile unsigned int* timer_status_ptr = (unsigned int *) (TIMER_BASE + 0x00);
volatile unsigned int* timer_control_ptr = (unsigned int *) (TIMER_BASE + 0x04);
volatile unsigned int* timer_periodl_ptr = (unsigned int *) (TIMER_BASE + 0x08);
volatile unsigned int* timer_periodh_ptr = (unsigned int *) (TIMER_BASE + 0x0C);

volatile unsigned int leds_state = 0;


short segmentos(short digito) {
    switch (digito) {
    case 0: return 64;
    case 1: return 121;
    case 2: return 36;
    case 3: return 48;
    case 4: return 25;
    case 5: return 18;
    case 6: return 2;
    case 7: return 120;
    case 8: return 0;
    case 9: return 24;
    default: return 127;
    }
}

// ISR botones

void button_isr_handler(void* context, alt_u32 id) {
    unsigned int buttons = *buttons_edge_ptr;
    *buttons_edge_ptr = 0;

    // Toggle LEDs
    leds_state ^= buttons;

    *leds_ptr = leds_state;

    alt_putstr("ISR BOTONES ejecutada\n");
}


// ISR timer
void timer_isr_handler(void* context, alt_u32 id) {
    *timer_status_ptr = 0; // Limpiar status

    segundos++;
    if (segundos >= 60) {
        segundos = 0;
        minutos++;
    }
    if (minutos >= 99) {
        minutos = 0;
    }

    actualizar_display = 1;

    alt_putstr("ISR TIMER ejecutada\n");
}


void mostrar_duracion(int minutos, int segundos) {
    int min_dec = minutos / 10;
    int min_uni = minutos % 10;
    int seg_dec = segundos / 10;
    int seg_uni = segundos % 10;

    unsigned int display_value =
        (segmentos(min_dec) << 24) |
        (segmentos(min_uni) << 16) |
        (segmentos(seg_dec) << 8) |
        (segmentos(seg_uni));

    *segments_ptr = display_value;
}

// Main
int main() {
    alt_putstr("Inicio del programa\n");

    // Config botones
    *buttons_edge_ptr = 0;
    *buttons_mask_ptr = 0xF; // Habilitar interrupciones botones 0-3

    alt_irq_register(REG_BUTTONS_IRQ, NULL, button_isr_handler);

    *timer_periodl_ptr = TIMER_PERIOD & 0xFFFF;
    *timer_periodh_ptr = (TIMER_PERIOD >> 16) & 0xFFFF;

    *timer_control_ptr = 0x7; // START + CONT + ITO

    alt_irq_register(TIMER_IRQ, NULL, timer_isr_handler);

    while (1) {
        if (actualizar_display) {
            mostrar_duracion(minutos, segundos);
            actualizar_display = 0;
        }
    }

    return 0;
}


