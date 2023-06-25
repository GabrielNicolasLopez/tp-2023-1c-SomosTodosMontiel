#include "bitmap.h"

// BITMAP
struct stat stats_fd_bitmap;
char* p_bitmap;
t_bitarray* bitA_bitmap;

void crear_bitmap() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BITMAP, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < config_SupBloque->BLOCK_COUNT/8; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bitmap de %i bytes", config_SupBloque->BLOCK_COUNT/8);

    fclose(archivo);
}

void levantar_bitmap()
{
    log_info(logger, "Levantando BITMAP");
    int fd_bitmap = open(configFS->PATH_BITMAP, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bitmap
    fstat(fd_bitmap, &stats_fd_bitmap);

    p_bitmap = mmap(NULL, stats_fd_bitmap.st_size, PROT_WRITE, MAP_SHARED, fd_bitmap, 0);
    close(fd_bitmap);
    bitA_bitmap = bitarray_create_with_mode(p_bitmap, stats_fd_bitmap.st_size, LSB_FIRST);
}

uint32_t get_free_block()
{
    uint32_t block = 0;
    uint32_t MAX = bitarray_get_max_bit(bitA_bitmap);
    while (block <= MAX && !bitarray_test_bit(bitA_bitmap, block)) {
        block++;
    }

    if (block > MAX) {
        log_error(logger, "No hay bloques libres!!!"); 
        return -1;
    }

    bitarray_set_bit(bitA_bitmap, block);
    return block;
}



