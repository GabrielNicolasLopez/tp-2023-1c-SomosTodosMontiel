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

int escribir_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes, uint8_t* cadena_bytes)
{
    // VER ESQUEMA DE MI CUADERNO PARA ENTENER LA LÓGICA
    
    //CONSTANTES:
    int block_size = config_SupBloque->BLOCK_SIZE;

    //VARIABLES:
    int bloque_contiene_p_archivo = ceil((double) cant_bytes / block_size);
    int offset = (((double) cant_bytes / block_size) - bloque_contiene_p_archivo) * block_size;
    int restante = (cant_bytes > block_size - offset)
                    ? block_size - offset
                    : cant_bytes;
    int enteros = floor((double) (cant_bytes - restante) / block_size);
    int sobrante = cant_bytes - restante - enteros;
    uint32_t bytes_escritos = 0;
    uint32_t bloque_a_escribir;
    

    // ESCRIBO RESTANTE
    bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo, FCB);
    escribir_bloque(bloque_a_escribir, offset, cadena_bytes + bytes_escritos, restante);
    bytes_escritos += restante;

    // ESCRIBO ENTEROS
    if (enteros) {
        for (int i = 1; i <= enteros; i++) {
            bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo + i, FCB);
            escribir_bloque(bloque_a_escribir, 0, cadena_bytes + bytes_escritos, block_size);
            bytes_escritos += block_size; 
        }
    }
    
    // ESCRIBO SOBRANTE
    if (sobrante) {
        bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo + enteros + 1, FCB);
        escribir_bloque(bloque_a_escribir, 0, cadena_bytes + bytes_escritos, sobrante);
        bytes_escritos += sobrante; 
    }

    msync(p_bloques, stats_fd_bloques.st_size, MS_SYNC);
    return bytes_escritos;
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

uint8_t* leer_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes)
{
    // VER ESQUEMA DE MI CUADERNO PARA ENTENER LA LÓGICA
    
    //CONSTANTES:
    int block_size = config_SupBloque->BLOCK_SIZE;

    //VARIABLES:
    int bloque_contiene_p_archivo = ceil((double) cant_bytes / block_size);
    int offset = (((double) cant_bytes / block_size) - bloque_contiene_p_archivo) * block_size;
    int restante = (cant_bytes > block_size - offset)
                    ? block_size - offset
                    : cant_bytes;
    int enteros = floor((double) (cant_bytes - restante) / block_size);
    int sobrante = cant_bytes - restante - enteros;
    
    uint8_t* cadena_bytes = malloc(cant_bytes);
    uint32_t bytes_leidos = 0;
    uint32_t bloque_a_leer;

    // LEO RESTANTE
    bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo, FCB);
    leer_bloque(bloque_a_leer, offset, cadena_bytes + bytes_leidos, restante);
    bytes_leidos += restante;

    // LEO ENTEROS
    if (enteros) {
        for (int i = 1; i <= enteros; i++) {
            bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo + i, FCB);
            escribir_bloque(bloque_a_leer, 0, cadena_bytes + bytes_leidos, block_size);
            bytes_leidos += block_size;
        }
    }

    // LEO SOBRANTE
    if (sobrante) {
        bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo + enteros + 1, FCB);
        escribir_bloque(bloque_a_leer, 0, cadena_bytes + bytes_leidos, sobrante);
        bytes_leidos += sobrante; 
    }

    return cadena_bytes;
}


uint32_t buscar_bloque(int numero_bloque, t_lista_FCB_config* FCB)
{
    if (numero_bloque == 1) {
        return FCB->FCB_config->PUNTERO_DIRECTO;
    }
    
    uint32_t puntero;
    int tamanio_puntero = sizeof(puntero);
    off_t offset = (numero_bloque - 2) * tamanio_puntero;
    
    leer_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, offset, &puntero, tamanio_puntero);
    return puntero;
}
