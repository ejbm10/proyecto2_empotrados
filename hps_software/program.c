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

int paused = 0;
int current = 0;
int running = 1;  // Asegúrate de definir running

void next_song() {
    current = (current + 1) % 3;
}

void prev_song() {
    current = (current + 2) % 3;  // Para evitar números negativos
}

void play_pause() {
    paused = !paused;
}

int main() {
    char* songs[3] = {
        "Kiss-I Was Made For Lovin You-1979.wav",
        "Haddaway-What is Love-1993.wav",
        "Alan Walker-Faded-2016.wav"
    };

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

    while (running) {
        printf("Escuchando: %s\n", songs[current]);

        FILE *fp = fopen(songs[current], "rb");
        if (!fp) {
            perror("fopen wav");
            munmap(map_base0, MAP_SIZE);
            munmap(map_base1, MAP_SIZE);
            close(fd);
            return -1;
        }

        fseek(fp, 44, SEEK_SET); // Saltar cabecera WAV

        uint16_t sample;
        int local = current;

        while (fread(&sample, sizeof(uint16_t), 1, fp) == 1) {

            if ((fifo1_ptr[0] & 0xf) != 0) {
                if (fifo1_ptr[0] == 0x8) play_pause();
                else if (fifo1_ptr[0] == 0x2) prev_song();
                else if (fifo1_ptr[0] == 0x1) next_song();
            }

            if (current != local) break;

            while (paused) {
                if (current != local)
                    break;
            }

            fifo0_ptr[0] = (uint32_t) sample;
            printf("Sent: %x\n", sample);
            usleep(10000);
        }

        fclose(fp);
        next_song();
    }

    munmap(map_base0, MAP_SIZE);
    munmap(map_base1, MAP_SIZE);
    close(fd);

    return 0;
}