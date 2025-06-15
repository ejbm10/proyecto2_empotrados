#include "io.h"
#include "system.h"
#include <string.h>
#include <stdio.h>

#define CHAR_BUFFER_BASE  0x01020000
#define CHAR_CONTROL_BASE 0x01000100
#define CLEAR_COMMAND     0x01
#define ENABLE_COMMAND    0x02
#define MAX_COLS          60
#define MAX_ROWS          80

void vga_clear() {
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, CLEAR_COMMAND);
}

void vga_enable() {
    IOWR_32DIRECT(CHAR_CONTROL_BASE, 0, ENABLE_COMMAND);
}

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

void update_info(const char *cantante, const char *titulo, const char *anio) {
    char linea[81];
    snprintf(linea, sizeof(linea), "Cantante: %.64s", cantante);
    vga_write_string(8, 2, linea);

    snprintf(linea, sizeof(linea), "Titulo  : %.64s", titulo);
    vga_write_string(16, 4, linea);

    snprintf(linea, sizeof(linea), "Ano     : %.64s", anio);
    vga_write_string(24, 6, linea);
}

// --- NUEVA FUNCION PARA EXTRAER DATOS ---
void extraer_info(const char *filename, char *cantante, char *titulo, char *anio) {
    // Copia el nombre porque strtok modifica la cadena
    char temp[128];
    strncpy(temp, filename, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    // Extrae con strtok
    char *token = strtok(temp, "-");
    if (token) strncpy(cantante, token, 64); else cantante[0] = '\0';

    token = strtok(NULL, "-");
    if (token) strncpy(titulo, token, 64); else titulo[0] = '\0';

    token = strtok(NULL, "-");
    if (token) {
        // Quita extensión ".wav" si la tiene
        char *dot = strchr(token, '.');
        if (dot) *dot = '\0';
        strncpy(anio, token, 16);
    } else {
        anio[0] = '\0';
    }
}

int main() {
    vga_clear();
    vga_enable();

    char cantante[65], titulo[65], anio[17];

    // ---- PRIMER EJEMPLO ----
    const char *fileinfo = "Kiss-I Was Made For Lovin' U-1979.wav";
    extraer_info(fileinfo, cantante, titulo, anio);
    update_info(cantante, titulo, anio);

    // Cambia datos tras delay largo
    volatile int delay;
    for (delay = 0; delay < 30000000; delay++);

    // ---- SEGUNDO EJEMPLO ----
    fileinfo = "Queen-Bohemian Rhapsody-1975.wav";
    extraer_info(fileinfo, cantante, titulo, anio);
    update_info(cantante, titulo, anio);

    while (1);
    return 0;
}

