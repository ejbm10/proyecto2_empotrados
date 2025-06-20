#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define FIFO_0_BASE   0xFF202870
#define FIFO_1_BASE   0xFF202904
#define FIFO_2_BASE   0xFF203000

#define MAP_SIZE 4096

volatile uint32_t *fifo0_ptr;
volatile uint32_t *fifo1_ptr;
volatile uint32_t *fifo2_ptr;

pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;

int current = 0;
char* songs[3] = {
    "song1.wav",
    "song2.wav",
    "song3.wav"
};

void send_string(const char* str) {
    while (*str) {
        pthread_mutex_lock(&mux);
        fifo2_ptr[0] = (uint32_t)(*str);  // Enviar byte
        pthread_mutex_unlock(&mux);

        str++;
    }
}

uint32_t read_uint32_le(FILE *f) {
    uint8_t b[4];
    fread(b, 1, 4, f);
    return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

char* read_chunk_data(FILE *f, uint32_t size) {
    char *data = malloc(size + 1);
    if (!data) {
        fseek(f, size, SEEK_CUR);
        return NULL;
    }

    fread(data, 1, size, f);
    data[size] = '\0';

    // Limpiar caracteres no imprimibles
    int i;
    for (i = size - 1; i >= 0; i--) {
        if ((unsigned char)data[i] < 32 || (unsigned char)data[i] > 126)
            data[i] = '\0';
        else
            break;
    }

    // Alinear a 2 bytes si es necesario
    if (size % 2 != 0) fseek(f, 1, SEEK_CUR);

    return data;
}

void read_wav_metadata(FILE* f) {
    char id[5] = {0};
    fread(id, 1, 4, f); // "RIFF"
    if (strncmp(id, "RIFF", 4) != 0) {
        printf("No es un archivo RIFF válido\n");
        fclose(f);
    }

    printf("Lei el RIFF\n");

    fseek(f, 4, SEEK_CUR); // skip chunk size
    fread(id, 1, 4, f);    // "WAVE"
    if (strncmp(id, "WAVE", 4) != 0) {
        printf("No es un archivo WAVE válido\n");
        fclose(f);
    }

    printf("Lei el WAVE\n");

    // Variables para guardar metadata
    char *title = NULL;
    char *artist = NULL;
    char *year = NULL;
    char *album = NULL;
    char *genre = NULL;
    char *software = NULL;

    printf("Cree las variables\n");

    // Buscar chunk LIST con tipo INFO
    while (fread(id, 1, 4, f) == 4) {
        uint32_t size = read_uint32_le(f);
        id[4] = '\0';

        if (strncmp(id, "LIST", 4) == 0) {
            char type[5] = {0};
            fread(type, 1, 4, f);
            size -= 4; // ya leímos parte del chunk

            printf("Salto LIST\n");

            if (strncmp(type, "INFO", 4) == 0) {
                long list_end = ftell(f) + size;

                while (ftell(f) < list_end) {
                    char sub_id[5] = {0};
                    if (fread(sub_id, 1, 4, f) != 4) break;
                    uint32_t sub_size = read_uint32_le(f);
                    sub_id[4] = '\0';

                    char *data = read_chunk_data(f, sub_size);
                    if (!data) continue;

                    if (strcmp(sub_id, "INAM") == 0) title = data;
                    else if (strcmp(sub_id, "IART") == 0) artist = data;
                    else if (strcmp(sub_id, "ICRD") == 0) year = data;
                    else if (strcmp(sub_id, "IPRD") == 0) album = data;
                    else if (strcmp(sub_id, "IGNR") == 0) genre = data;
                    else if (strcmp(sub_id, "ISFT") == 0) software = data;
                    else free(data); // chunk no usado

                    printf("Asigno chunks\n");
                }
            } else {
                fseek(f, size, SEEK_CUR);
            }
        } else {
            fseek(f, size, SEEK_CUR);
        }
    }

    // Enviar metadata recogida
    if (title) {
        send_string(title);
        send_string("-");
    }
    if (artist) {
        send_string(artist);
        send_string("-");
    }
    if (year) {
        send_string(year);
        send_string("-");
    }
    if (album) {
        send_string(album);
        send_string("-");
    }
    if (genre) {
        send_string(genre);
        send_string("-");
    }
    if (software) {
        send_string(software);
    }
    send_string("/");

    printf("Ya envie la metadata\n");

    // Liberar memoria
    free(title);
    free(artist);
    free(year);
    free(album);
    free(genre);
    free(software);

    printf("Ya libere la memoria\n");
}

void send_audio_data(FILE *f) {
    char id[5] = {0};
    uint32_t size = 0;
    long pos = 0;

    // Saltar encabezado RIFF y WAVE
    fseek(f, 12, SEEK_SET); // RIFF (4) + size (4) + WAVE (4)

    while (fread(id, 1, 4, f) == 4) {
        size = read_uint32_le(f);
        id[4] = '\0';

        if (strcmp(id, "data") == 0) {
            printf("Encontrado chunk 'data', tamaño: %u bytes\n", size);
            pos = ftell(f);
            break;
        } else {
            // Saltar este chunk
            fseek(f, size, SEEK_CUR);
        }
    }

    if (pos == 0) {
        printf("No se encontró el chunk 'data'\n");
        return;
    }

    // Volver al inicio del chunk data
    fseek(f, pos, SEEK_SET);

    // Enviar muestras 16-bit (2 bytes por muestra)
    int i;
    fifo0_ptr[0] = size;
    for (i = 0; i < size; i += 2) {
        uint8_t buf[2];
        if (fread(buf, 1, 2, f) != 2) break;

        uint16_t sample = buf[0] | (buf[1] << 8);

        fifo0_ptr[0] = sample;
    }

    printf("Se enviaron %u muestras de audio\n", size / 2);
}

void* songs_handler(void* args) {
    while (1) {
        FILE* f = fopen(songs[current], "rb");
        read_wav_metadata(f);
        send_audio_data(f);
        fclose(f);

        current = (current + 1) % 3;
    }
}

int main(int argc, char* argv[]) {
    current = atoi(argv[1]);

    // Abre la memoria
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

    // Mapeo para FIFO 2
    void *map_base2 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FIFO_2_BASE & ~(MAP_SIZE - 1));
    if (map_base2 == MAP_FAILED) {
        perror("mmap fifo2");
        munmap(map_base1, MAP_SIZE);
        close(fd);
        return -1;
    }
    fifo2_ptr = (volatile uint32_t *) ((uint8_t*)map_base2 + (FIFO_2_BASE & (MAP_SIZE - 1)));

    // Crea el thread que constantemente listen el FIFO de botones
    pthread_t t1;//, t2;
    
    pthread_create(&t1, NULL, songs_handler, NULL);
    //pthread_create(&t2, NULL, button_listener, NULL);

    pthread_join(t1, NULL);
    //pthread_join(t2, NULL);

    return 0;
}
