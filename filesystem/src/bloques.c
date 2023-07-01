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

int asignar_bloques(t_lista_FCB_config* FCB, uint32_t bytes)
{
    // TAMAÑO ANTERIOR DEL ARCHIVO
    uint32_t tam_ant_arch = FCB->FCB_config->TAMANIO_ARCHIVO;
    // TAMAÑO NUEVO DEL ARCHIVO
    uint32_t tam_nuevo_arch = bytes;

    // CONSTANTES DE MI FS:
        // TAMAÑO DE BLOQUE DE DATOS
        uint32_t tam_bloque = config_SupBloque->BLOCK_SIZE;
        // TAMAÑO DEL PUNTERO
        uint32_t tam_puntero = sizeof(uint32_t);
        // CANTIDAD DE PUNTEROS POR BLOQUE
        uint32_t cant_punt_x_bloque = tam_bloque / tam_puntero;
        // TAMAÑO MAXIMO REAL DEL ARCHIVO
        uint32_t tam_max_arch = minimum(tam_bloque * (1 + 1 * cant_punt_x_bloque), tam_bloque * config_SupBloque->BLOCK_COUNT);

    if (tam_nuevo_arch > tam_max_arch) {
        log_error(logger, "TRUNCAR: Tamaño de asignacion invalido, archivo: %s", FCB->nombre_archivo); 
        return -1;
    }

    // CANTIDAD DE BLOQUES YA OCUPADOS
    uint32_t bloques_ya_ocupados = ceil((double) tam_ant_arch / tam_bloque);
    // CANTIDAD DE BLOQUES TOTALES A OCUPAR
    uint32_t bloques_totales_a_ocupar = ceil((double) tam_nuevo_arch / tam_bloque);
    // CANTIDAD DE BLOQUES NUEVOS A OCUPAR
    uint32_t bloques_nuevos_a_ocupar = bloques_totales_a_ocupar - bloques_ya_ocupados;
    
    config_set_value(FCB->config, "TAMANIO_ARCHIVO", string_itoa(tam_nuevo_arch));
    
    // Casos dondo no hay asignado un puntero directo
    if (tam_ant_arch == 0 && bloques_nuevos_a_ocupar >= 1) {
        // Asigno puntero directo
        config_set_value(FCB->config, "PUNTERO_DIRECTO", string_itoa(get_free_block()));
        bloques_nuevos_a_ocupar--;
    } 
    if (bloques_totales_a_ocupar > 1) {
        // Casos donde no hay asignado un puntero indirecto
        if (tam_ant_arch <= tam_bloque) {
            uint32_t bloque_punteros = get_free_block();
            // Asigno puntero indirecto
            config_set_value(FCB->config, "PUNTERO_INDIRECTO", string_itoa(bloque_punteros));
            actualizar_FCB(FCB);
        }


        if (bloques_nuevos_a_ocupar > 0) {
            //Ultimo puntero del bloque de punteros asignado
            uint32_t ultimo_p_indirecto = max( (int) bloques_ya_ocupados - 1, 0) ;

            // Asigno la cantidad de punteros a bloques de datos que necesito
            uint32_t bloque_a_ocupar;
            for(int i = 0; i < bloques_nuevos_a_ocupar; i++) {
                bloque_a_ocupar = get_free_block();
                // Asigno puntero en bloque de punteros
                escribir_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, ultimo_p_indirecto * tam_puntero, &bloque_a_ocupar, tam_puntero);
                ultimo_p_indirecto ++;
            }
        }
    }
    
    actualizar_FCB(FCB);
    msync(p_bitmap, stats_fd_bitmap.st_size, MS_SYNC);
    msync(p_bloques, stats_fd_bloques.st_size, MS_SYNC);

    return 0;
}

int liberar_bloques(t_lista_FCB_config* FCB, uint32_t bytes)
{
    // TAMAÑO ANTERIOR DEL ARCHIVO
    uint32_t tam_ant_arch = FCB->FCB_config->TAMANIO_ARCHIVO;
    // TAMAÑO NUEVO DEL ARCHIVO
    uint32_t tam_nuevo_arch = bytes;
    

    // CONSTANTES DE MI FS:
        // TAMAÑO DE BLOQUE DE DATOS
        uint32_t tam_bloque = config_SupBloque->BLOCK_SIZE;
        // TAMAÑO DEL PUNTERO
        uint32_t tam_puntero = sizeof(uint32_t);
        // TAMAÑO MAXIMO REAL DEL ARCHIVO
        uint32_t tam_min_arch = 0;

    if (tam_nuevo_arch < tam_min_arch) {
        log_error(logger, "TRUNCAR: Tamaño de liberacion invalido, archivo: %s", FCB->nombre_archivo);
        return -1;
    }

    // CANTIDAD DE BLOQUES YA OCUPADOS
    uint32_t bloques_ocupados_ant = ceil((double) tam_ant_arch / tam_bloque);
    // CANTIDAD DE BLOQUES TOTALES A OCUPAR
    uint32_t bloques_ocupados_nuevo = ceil((double) tam_nuevo_arch / tam_bloque);
    // CANTIDAD DE BLOQUES NUEVOS A OCUPAR
    uint32_t bloques_a_liberar = bloques_ocupados_ant - bloques_ocupados_nuevo;

    config_set_value(FCB->config, "TAMANIO_ARCHIVO", string_itoa(tam_nuevo_arch));
    actualizar_FCB(FCB);

    if (bloques_ocupados_nuevo == 0) {
        bitarray_clean_bit(bitA_bitmap, FCB->FCB_config->PUNTERO_DIRECTO);
        bloques_a_liberar--;
    }

    if (bloques_ocupados_ant > 1) {
        //Ultimo puntero del bloque de punteros asignado
        uint32_t ultimo_p_indirecto = max( (int) bloques_ocupados_ant - 1, 0) ;
        
        uint32_t bloque_a_liberar;
        uint32_t bloque_default = 0;
        for (int i = 0; i < bloques_a_liberar; i++) {
            leer_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, (ultimo_p_indirecto-1) * tam_puntero, &bloque_a_liberar, tam_puntero);
            escribir_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, (ultimo_p_indirecto-1) * tam_puntero, &bloque_default, tam_puntero);
            bitarray_clean_bit(bitA_bitmap, bloque_a_liberar);
            ultimo_p_indirecto--;
        }

        if (bloques_ocupados_nuevo <= 1) {
            bitarray_clean_bit(bitA_bitmap, FCB->FCB_config->PUNTERO_INDIRECTO);
        }
    }
    
    
    msync(p_bitmap, stats_fd_bitmap.st_size, MS_SYNC);
    msync(p_bloques, stats_fd_bloques.st_size, MS_SYNC);

    return 0;
}

uint32_t minimum(uint32_t x, uint32_t y)
{
    if (x > y)
        return y;
    return x;
}

int max(int x, int y)
{
    if (x < y)
        return y;
    return x;
}
