#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define FIFO_0_BASE   0xFF202870
#define FIFO_1_BASE   0xFF202874

#define MAP_SIZE    4096

volatile uint32_t *fifo0_ptr;
volatile uint32_t *fifo1_ptr;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo>", argv[1]);
    }

    const char* song = argv[1];

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem");
        return -1;
    }

    // Mapeo para FIFO 0
    void *map_base0 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FIFO_0_BASE & ~(MAP_SIZE - 1));
    if (map_base0 == MAP_FAILED) {
        perror("mmap fifo0");
        close(fd);
        return -1;
    }
    fifo0_ptr = (volatile uint32_t *) ((uint8_t*)map_base0 + (FIFO_0_BASE & (MAP_SIZE - 1)));

    // Mapeo para FIFO 1
    void *map_base1 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FIFO_1_BASE & ~(MAP_SIZE - 1));
    if (map_base1 == MAP_FAILED) {
        perror("mmap fifo1");
        munmap(map_base0, MAP_SIZE);
        close(fd);
        return -1;
    }
    fifo1_ptr = (volatile uint32_t *) ((uint8_t*)map_base1 + (FIFO_1_BASE & (MAP_SIZE - 1)));

    int paused = 0;
    int current = 0;
    
    while (1) {
        printf("Escuchando: %s\n", song);

        FILE *fp = fopen(song, "rb");
        if (!fp) {
            perror("Nombre incorrecto del archivo");
            munmap(map_base0, MAP_SIZE);
            munmap(map_base1, MAP_SIZE);
            close(fd);
            return -1;
        }

        printf("Archivo abierto correctamente\n"); fflush(stdout);

        int r = fseek(fp, 44, SEEK_SET); // Saltar cabecera WAV
        
        printf("Resultado de fseek: %d\n", r); fflush(stdout);

        uint16_t sample;

        printf("Current: %d\n", current);

        while (fread(&sample, sizeof(uint16_t), 1, fp) == 1) {
            unsigned int cmd = fifo1_ptr[0];

            if (cmd == 0x8 || cmd == 0x4) {
                paused = !paused;
            } else if (cmd == 0x2) {
                current--;
                if (current < 0) current = 2;
                break;
            } else if (cmd == 0x1) {
                current++;
                if (current > 2) current = 0;
                break; 
            }

            fifo0_ptr[0] = (uint32_t) sample;
            printf("Sent: 0x%X\n", sample);
            usleep(10000);
        }

        fclose(fp);
    }

    munmap(map_base0, MAP_SIZE);
    munmap(map_base1, MAP_SIZE);
    close(fd);

    return 0;
}