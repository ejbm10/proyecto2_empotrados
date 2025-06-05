#include "io.h"
#include "system.h"
#include <string.h>

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02

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

void delay()
{
    volatile int i;
    for (i = 0; i < 1000000; i++);
}

int main()
{
    const char *sentence = "Hola, esta es una oracion larga con mas de quince palabras para la pantalla VGA.";

    int i;

    // Clear
    vga_clear();
    delay();
    vga_enable();
    delay();

    // Escribir la oración en pantalla
    for (i = 0; i < strlen(sentence); i++)
    {
    	vga_clear();
    	delay();
    	vga_enable();
    	delay();
        vga_write_char(5 + i, 1, sentence[i]);  // fila Y=10
    }

    // Infinite loop
    while (1);

    return 0;
}


