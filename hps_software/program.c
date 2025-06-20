#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define FIFO_1_BASE   0xC0002904

#define MAP_SIZE    4096

volatile uint32_t *fifo1_ptr;

void* button_listener(void* args) {
    while (1) {
        unsigned int pressed = fifo1_ptr[0];
        printf("Button pressed: %x\n", pressed);

        if (pressed == 8 || pressed == 4) printf("Pausa\n");
        else if (pressed == 2) printf("Previous\n");
        else if (pressed == 1) printf("Next\n");
    }
}

void* otro_thread(void* args) {
    while (1) {
        printf("Hello World!!!!\n");
    }
}

int main() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem");
        return -1;
    }

    // Mapeo para FIFO 1
    void *map_base1 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FIFO_1_BASE & ~(MAP_SIZE - 1));
    if (map_base1 == MAP_FAILED) {
        perror("mmap fifo1");
        close(fd);
        return -1;
    }
    fifo1_ptr = (volatile uint32_t *) ((uint8_t*)map_base1 + (FIFO_1_BASE & (MAP_SIZE - 1)));

    pthread_t t1, t2;

    pthread_create(&t1, NULL, button_listener, NULL);
    pthread_create(&t2, NULL, otro_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    munmap(map_base1, MAP_SIZE);
    close(fd);

    return 0;
}