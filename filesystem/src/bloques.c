#include "bloques.h"

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
    //sleep(configFS->RETARDO_ACCESO_BLOQUE / 1000);
    
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

int escribir_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes, char* cadena_bytes)
{
    // VER ESQUEMA DE MI CUADERNO PARA ENTENER LA LÓGICA
    
    //CONSTANTES:
    int block_size = config_SupBloque->BLOCK_SIZE;

    //VARIABLES:
    double bloque_p_a = (double) puntero_archivo / block_size;
    
    int bloque_contiene_p_archivo = (puntero_archivo % block_size == 0)
                                    ? bloque_p_a + 1
                                    : ceil(bloque_p_a);
    int offset = (bloque_p_a - floor(bloque_p_a)) * block_size;
    int restante = (cant_bytes > block_size - offset)
                    ? block_size - offset
                    : cant_bytes;
    int enteros = floor((double) (cant_bytes - restante) / block_size);
    int sobrante = cant_bytes - restante - enteros;
    uint32_t bytes_escritos = 0;
    uint32_t bloque_a_escribir;
    
    uint32_t* PIS = NULL;
    if (FCB->FCB_config->PUNTERO_INDIRECTO != -1) {
        PIS = leer_PIS(FCB);
    }

    // ESCRIBO RESTANTE
    bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo, FCB, PIS);
    log_info(logger, "Acceso Bloque (W) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
        FCB->nombre_archivo,
        bloque_contiene_p_archivo,
        bloque_a_escribir
    );
    escribir_bloque(bloque_a_escribir, offset, cadena_bytes + bytes_escritos, restante);
    bytes_escritos += restante;

    // ESCRIBO ENTEROS
    if (enteros) {
        for (int i = 1; i <= enteros; i++) {
            bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo + i, FCB, PIS);
            log_info(logger, "Acceso Bloque (W) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
                FCB->nombre_archivo,
                bloque_contiene_p_archivo + i,
                bloque_a_escribir
            );
            escribir_bloque(bloque_a_escribir, 0, cadena_bytes + bytes_escritos, block_size);
            bytes_escritos += block_size; 
        }
    }
    
    // ESCRIBO SOBRANTE
    if (sobrante) {
        bloque_a_escribir = buscar_bloque(bloque_contiene_p_archivo + enteros + 1, FCB, PIS);
        log_info(logger, "Acceso Bloque (W) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
            FCB->nombre_archivo,
            bloque_contiene_p_archivo + enteros + 1,
            bloque_a_escribir
        );
        escribir_bloque(bloque_a_escribir, 0, cadena_bytes + bytes_escritos, sobrante);
        bytes_escritos += sobrante; 
    }

    free(PIS);
    msync(p_bloques, stats_fd_bloques.st_size, MS_SYNC);
    return bytes_escritos;
}

int leer_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio)
{
    //sleep(configFS->RETARDO_ACCESO_BLOQUE / 1000);
    
    if (offset + tamanio > config_SupBloque->BLOCK_SIZE) {
        log_error(logger, "LECTURA DE BLOQUE %u SOBREPASA EL TAMAÑO", bloque);
        return -1;
    }

    // BYTE 0 DEL BLOQUE DONDE VAMOS A LEER
    off_t inicio_bloque = bloque * config_SupBloque->BLOCK_SIZE;

    memcpy(reg, p_bloques + inicio_bloque + offset, tamanio);

    return 0;
}

char* leer_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes)
{
    // VER ESQUEMA DE MI CUADERNO PARA ENTENER LA LÓGICA
    
    //CONSTANTES:
    int block_size = config_SupBloque->BLOCK_SIZE;

    //VARIABLES:
    double bloque_p_a = (double) puntero_archivo / block_size;

    int bloque_contiene_p_archivo = (puntero_archivo % block_size == 0)
                                    ? bloque_p_a + 1
                                    : ceil(bloque_p_a);
    int offset = (bloque_p_a - floor(bloque_p_a)) * block_size;
    int restante = (cant_bytes > block_size - offset)
                    ? block_size - offset
                    : cant_bytes;
    int enteros = floor((double) (cant_bytes - restante) / block_size);
    int sobrante = cant_bytes - restante - enteros;
    
    char* cadena_bytes = malloc(cant_bytes);
    uint32_t bytes_leidos = 0;
    uint32_t bloque_a_leer;

    uint32_t* PIS = NULL;
    if (FCB->FCB_config->PUNTERO_INDIRECTO != -1) {
        PIS = leer_PIS(FCB);
    }

    // LEO RESTANTE
    bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo, FCB, PIS);
    log_info(logger, "Acceso Bloque (R) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
        FCB->nombre_archivo,
        bloque_contiene_p_archivo,
        bloque_a_leer
    );
    leer_bloque(bloque_a_leer, offset, cadena_bytes + bytes_leidos, restante);
    bytes_leidos += restante;

    // LEO ENTEROS
    if (enteros) {
        for (int i = 1; i <= enteros; i++) {
            bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo + i, FCB, PIS);
            log_info(logger, "Acceso Bloque (R) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
                FCB->nombre_archivo,
                bloque_contiene_p_archivo  + i,
                bloque_a_leer
            );
            leer_bloque(bloque_a_leer, 0, cadena_bytes + bytes_leidos, block_size);
            bytes_leidos += block_size;
        }
    }

    // LEO SOBRANTE
    if (sobrante) {
        bloque_a_leer = buscar_bloque(bloque_contiene_p_archivo + enteros + 1, FCB, PIS);
        log_info(logger, "Acceso Bloque (R) - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%u>",
            FCB->nombre_archivo,
            bloque_contiene_p_archivo + enteros + 1,
            bloque_a_leer
        );
        leer_bloque(bloque_a_leer, 0, cadena_bytes + bytes_leidos, sobrante);
        bytes_leidos += sobrante; 
    }

    free(PIS);
    return cadena_bytes;
}

uint32_t* leer_PIS(t_lista_FCB_config* FCB)
{
    uint32_t* PIS = malloc(config_SupBloque->BLOCK_SIZE);
    
    log_info(logger, "Acceso Bloque (R) - Archivo: <%s> - Bloque Archivo: <Punt. Indirecto> - Bloque File System <%u>",
        FCB->nombre_archivo,
        FCB->FCB_config->PUNTERO_INDIRECTO
    );
    leer_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, 0, PIS, config_SupBloque->BLOCK_SIZE);

    return PIS;
}

uint32_t buscar_bloque(int numero_bloque, t_lista_FCB_config* FCB, uint32_t* PIS)
{
    if (numero_bloque == 1) {
        return FCB->FCB_config->PUNTERO_DIRECTO;
    }
    
    uint32_t puntero;
    off_t offset = numero_bloque - 2;
    
    memcpy(&puntero, PIS + offset, sizeof(puntero));
    return puntero;
}

int asignar_bloques(t_lista_FCB_config* FCB, uint32_t bytes)
{
    log_debug(logger, "Asignando bloques:");

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
    
    char* s_tam_nuevo_arch = string_itoa(tam_nuevo_arch);
    config_set_value(FCB->config, "TAMANIO_ARCHIVO", s_tam_nuevo_arch);
    free(s_tam_nuevo_arch);
    
    // Casos dondo no hay asignado un puntero directo
    if (tam_ant_arch == 0 && bloques_nuevos_a_ocupar >= 1) {
        // Asigno puntero directo
        char* bloque_libre = string_itoa(get_free_block());
        config_set_value(FCB->config, "PUNTERO_DIRECTO", bloque_libre);
        free(bloque_libre);
        bloques_nuevos_a_ocupar--;
    } 
    if (bloques_totales_a_ocupar > 1) {
        // Casos donde no hay asignado un puntero indirecto
        if (tam_ant_arch <= tam_bloque) {
            uint32_t bloque_punteros = get_free_block();
            // Asigno puntero indirecto
            char* pis = string_itoa(bloque_punteros);
            config_set_value(FCB->config, "PUNTERO_INDIRECTO", pis);
            free(pis);
            actualizar_FCB(FCB);
        }


        if (bloques_nuevos_a_ocupar > 0) {
            //Ultimo puntero del bloque de punteros asignado
            uint32_t ultimo_p_datos = max( (int) bloques_ya_ocupados - 1, 0) ;

            // PUNTEROS DE DATOS A ESCRIBIR EN EL PUNTERO INDIRECTO
            uint32_t* punteros_a_escribir = malloc(tam_puntero * bloques_nuevos_a_ocupar);

            // Asigno la cantidad de punteros a bloques de datos que necesito
            uint32_t bloque_libre;
            for(int i = 0; i < bloques_nuevos_a_ocupar; i++) {
                bloque_libre = get_free_block();
                punteros_a_escribir[i] = bloque_libre;
            }
            log_info(logger, "Acceso Bloque (W) - Archivo: <%s> - Bloque Archivo: <Punt. Indirecto> - Bloque File System <%u>",
                FCB->nombre_archivo,
                FCB->FCB_config->PUNTERO_INDIRECTO
            );
            escribir_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, ultimo_p_datos * tam_puntero, punteros_a_escribir, tam_puntero * bloques_nuevos_a_ocupar);
            free(punteros_a_escribir);
        }
    }
    
    actualizar_FCB(FCB);
    msync(p_bitmap, stats_fd_bitmap.st_size, MS_SYNC);
    msync(p_bloques, stats_fd_bloques.st_size, MS_SYNC);

    return 0;
}

int liberar_bloques(t_lista_FCB_config* FCB, uint32_t bytes)
{
    log_debug(logger, "Liberando bloques:");
    
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

    char* s_tam_nuevo_arch = string_itoa(tam_nuevo_arch);
    config_set_value(FCB->config, "TAMANIO_ARCHIVO", s_tam_nuevo_arch);
    free(s_tam_nuevo_arch);
    actualizar_FCB(FCB);

    if (bloques_ocupados_nuevo == 0) {
        log_info(logger, "Acceso a Bitmap - Bloque: <%u> - Estado: <%d>", FCB->FCB_config->PUNTERO_DIRECTO, 1);
        bitarray_clean_bit(bitA_bitmap, FCB->FCB_config->PUNTERO_DIRECTO);
        bloques_a_liberar--;
    }

    if (bloques_ocupados_ant > 1) {
        //Ultimo puntero del bloque de punteros asignado
        uint32_t ultimo_p_datos = max( (int) bloques_ocupados_ant - 1, 0) ;
        
        
        // PUNTEROS DE DATOS A LIBERAR EN EL PUNTERO INDIRECTO
        uint32_t* punteros_a_liberar = malloc(tam_puntero * bloques_a_liberar);
        
        // defino los punteros_a_liberar 
        log_info(logger, "Acceso Bloque (R) - Archivo: <%s> - Bloque Archivo: <Punt. Indirecto> - Bloque File System <%u>",
            FCB->nombre_archivo,
            FCB->FCB_config->PUNTERO_INDIRECTO
        );
        leer_bloque(FCB->FCB_config->PUNTERO_INDIRECTO, (ultimo_p_datos - bloques_a_liberar) * tam_puntero, punteros_a_liberar, bloques_a_liberar * tam_puntero);
        
        uint32_t bloque_a_liberar;
        for (int i = 0; i < bloques_a_liberar; i++) {
            bloque_a_liberar = punteros_a_liberar[i];
            log_info(logger, "Acceso a Bitmap - Bloque: <%u> - Estado: <%d>", bloque_a_liberar, 1);
            bitarray_clean_bit(bitA_bitmap, bloque_a_liberar);
        }
        free(punteros_a_liberar);

        if (bloques_ocupados_nuevo <= 1) {
            log_info(logger, "Acceso a Bitmap - Bloque: <%u> - Estado: <%d>", FCB->FCB_config->PUNTERO_INDIRECTO, 1);
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
