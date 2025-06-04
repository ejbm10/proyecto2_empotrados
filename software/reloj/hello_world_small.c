#include "io.h"
#include "system.h"

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02

void vga_clear()
{
    // Clear screen
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, CLEAR_COMMAND);
}

void vga_enable()
{
    // Enable DMA
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
    // Clear
    vga_clear();

    // Small delay
    delay();

    // Enable VGA DMA!
    vga_enable();

    // Another small delay
    delay();

    // Write "HELLO WORLD"
    vga_write_char(10, 5, 'H');
    vga_write_char(11, 5, 'E');
    vga_write_char(12, 5, 'L');
    vga_write_char(13, 5, 'L');
    vga_write_char(14, 5, 'O');

    vga_write_char(16, 5, 'P');
    vga_write_char(17, 5, 'O');
    vga_write_char(18, 5, 'R');
    vga_write_char(19, 5, 'L');
    vga_write_char(20, 5, 'D');

    // Infinite loop
    while (1);

    return 0;
}
