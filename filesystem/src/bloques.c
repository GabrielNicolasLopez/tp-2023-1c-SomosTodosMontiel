#include "bloques.h"

// BLOQUES
struct stat stats_fd_bloques;
char* p_bloques;

void crear_archivo_de_bloques() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BLOQUES, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < config_SupBloque->BLOCK_COUNT * config_SupBloque->BLOCK_SIZE; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bloques de %i bytes", config_SupBloque->BLOCK_COUNT * config_SupBloque->BLOCK_SIZE);

    fclose(archivo);
}

void levantar_bloques()
{
    log_info(logger, "Levantando BLOQUES");
    int fd_bloques = open(configFS->PATH_BLOQUES, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bloques
    fstat(fd_bloques, &stats_fd_bloques);

    p_bloques = mmap(NULL, stats_fd_bloques.st_size, PROT_WRITE, MAP_SHARED, fd_bloques, 0);
    close(fd_bloques);
}

int escribir_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio)
{
    if (offset > config_SupBloque->BLOCK_SIZE) {
        log_error(logger, "ESCRITURA DE BLOQUE: Offset en el bloque %u SOBREPASA EL TAMAÑO", bloque);
        return -1;
    }
    
    if (tamanio > config_SupBloque->BLOCK_SIZE) {
        tamanio = config_SupBloque->BLOCK_SIZE;
    }

    // BYTE 0 DEL BLOQUE DONDE VAMOS A ESCRIBIR
    off_t inicio_bloque = bloque * config_SupBloque->BLOCK_SIZE;

    memcpy(p_bloques + inicio_bloque + offset, reg, tamanio);


    return tamanio;
}

int leer_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio)
{
    if (offset + tamanio > config_SupBloque->BLOCK_SIZE) {
        log_error(logger, "LECTURA DE BLOQUE %u SOBREPASA EL TAMAÑO", bloque);
        return -1;
    }

    // BYTE 0 DEL BLOQUE DONDE VAMOS A LEER
    off_t inicio_bloque = bloque * config_SupBloque->BLOCK_SIZE;

    memcpy(reg, p_bloques + inicio_bloque + offset, tamanio);

    return 0;
}
