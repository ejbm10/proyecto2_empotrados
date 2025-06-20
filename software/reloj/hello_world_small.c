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

void vga_clear_manual() {
    for (int y = 0; y < MAX_ROWS; y++) {
        for (int x = 0; x < MAX_COLS; x++) {
            IOWR_8DIRECT(CHAR_BUFFER_BASE, (y * MAX_COLS + x), ' ');
        }
    }
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

// Separador de campos por "-" y elimina el "/" final del último campo
void extraer_campos(const char *info, char campos[][64], int *num_campos) {
    char temp[256];
    strncpy(temp, info, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char *token;
    int idx = 0;

    token = strtok(temp, "-");
    while (token != NULL && idx < 6) {
        // Quita "/" final si es el último campo
        char *slash = strchr(token, '/');
        if (slash) *slash = '\0';
        strncpy(campos[idx], token, 63);
        campos[idx][63] = '\0';
        idx++;
        token = strtok(NULL, "-");
    }
    *num_campos = idx;
}

// Usar coordenadas estilo desplazado como en tu ejemplo
void update_info_desplazada(char campos[][64], int num_campos) {
    // Coordenadas por campo (X, Y)
    const int x_offsets[] = {8, 16, 24, 32, 40, 48};  // puedes ajustar si tienes más campos
    const int y_start = 11;
    const int y_step  = 2;

    for (int i = 0; i < num_campos && i < 6; i++) {
        vga_write_string(x_offsets[i], y_start + i * y_step, campos[i]);
    }
}

int main() {
    vga_clear_manual();
    vga_enable();

    char campos[6][64];
    int num_campos = 0;

    // ---- PRIMER EJEMPLO ----
    const char *info1 = "I Was Made for Lovin' You-Kiss-1979-Dynasty-Hard Rock-Lavf61.1.100/";
    extraer_campos(info1, campos, &num_campos);
    update_info_desplazada(campos, num_campos);

    /*
    // --- Espera larga (simula cambio de canción) ---
    volatile int delay;
    for (delay = 0; delay < 30000000; delay++);

    // ---- SEGUNDO EJEMPLO ----
    const char *info2 = "Somebody to Love-Queen-1976-A Day at the Races-Rock-v1.0.0/";
    extraer_campos(info2, campos, &num_campos);
    update_info_desplazada(campos, num_campos);
	*/

    while (1);
    return 0;
}
