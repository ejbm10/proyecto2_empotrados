#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define FIFO_0_BASE   0xFF202870
#define FIFO_1_BASE   0xFF202904
#define FIFO_2_BASE   0xFF203000

#define MAP_SIZE    4096

#pragma pack(push, 1)
typedef struct {
    char     chunkID[4];      // "RIFF"
    uint32_t chunkSize;
    char     format[4];       // "WAVE"

    // Subchunk1
    char     subchunk1ID[4];  // "fmt "
    uint32_t subchunk1Size;   // 16 for PCM
    uint16_t audioFormat;     // PCM = 1
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    // Subchunk2
    char     subchunk2ID[4];  // "data"
    uint32_t subchunk2Size;
} WAVHeader;
#pragma pack(pop)

volatile uint32_t *fifo0_ptr;
volatile uint32_t *fifo1_ptr;

char* songs[3] = {
    "Kiss-I Was Made for Lovin You-1979.wav",
    "Haddaway-What is Love-1993.wav",
    "Alan Walker-Faded-2016.wav"
};

int paused = 0;
int next = 0;
int prev = 0;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*
void* button_reader(void* args) {
    while (1) {
        unsigned int val = fifo1_ptr[0];
        if (val == 8 || val == 4) {
            pthread_mutex_lock(&mutex);
            paused = !paused;
            pthread_mutex_unlock(&mutex);
        } else if (val == 2) {
            pthread_mutex_lock(&mutex);
            prev = 1;
            pthread_mutex_unlock(&mutex);
        } else if (val == 1) {
            pthread_mutex_lock(&mutex);
            next = 1;
            pthread_mutex_unlock(&mutex);
        }
    }
}*/

void* song_bytes_sender(void* args) {
    const char* filename = songs[0];
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("No se pudo abrir el archivo");
    }

    WAVHeader header;
    // Lee cabecera RIFF y fmt
    fread(&header, sizeof(WAVHeader), 1, file);

    // Verifica formato
    if (header.audioFormat != 1 || header.bitsPerSample != 16) {
        printf("Formato no soportado: audioFormat=%d, bitsPerSample=%d\n", header.audioFormat, header.bitsPerSample);
        fclose(file);
        return NULL;
    }

    // Buscar el chunk "data"
    char chunk_id[4];
    uint32_t chunk_size;
    while (fread(chunk_id, sizeof(char), 4, file) == 4) {
        fread(&chunk_size, sizeof(uint32_t), 1, file);

        if (strncmp(chunk_id, "data", 4) == 0) {
            break; // ¡Encontramos los datos reales!
        } else {
            // Saltar este chunk
            fseek(file, chunk_size, SEEK_CUR);
        }
    }

    if (strncmp(chunk_id, "data", 4) != 0) {
        printf("No se encontró el chunk de datos PCM\n");
        fclose(file);
        return NULL;
    }

    int total_samples = chunk_size / (header.bitsPerSample / 8);
    printf("Tamaño de datos: %d bytes\n", chunk_size);
    printf("Total de samples: %d\n\n", total_samples);
    /*
    else {
        printf("Canales       : %d\n", header.numChannels);
        printf("Sample Rate   : %d Hz\n", header.sampleRate);
        printf("Bits por muestra: %d\n", header.bitsPerSample);
        printf("Tamaño de datos: %d bytes\n", header.subchunk2Size);
        int total_samples = header.subchunk2Size / (header.bitsPerSample / 8);
        printf("Total de samples: %d\n\n", total_samples);

        int16_t sample;
        int i;
        for (i = 0; i < total_samples; i++) {
            if (fread(&sample, sizeof(int16_t), 1, file) != 1) break;
            printf("Sample %6d: 0x%04X (%6d)\n", i + 1, (uint16_t)sample, sample);
        }
    }*/

    fclose(file);
}

int main() {
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

    pthread_t songs_thread, buttons_thread;

    pthread_create(&songs_thread, NULL, song_bytes_sender, NULL);
    //pthread_create(&buttons_thread, NULL, button_reader, NULL);

    pthread_join(songs_thread, NULL);
    //pthread_join(buttons_thread, NULL);

    munmap(map_base0, MAP_SIZE);
    munmap(map_base1, MAP_SIZE);
    close(fd);

    return 0;
}