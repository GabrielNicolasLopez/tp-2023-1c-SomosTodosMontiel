#include "FCB.h"

int crear_carpeta_de_FCBs() 
{
    mode_t mode = 
        // OWNWER: Lectura, escritura y ejecucion
        S_IRWXU |
        // GROUP: Lectura, escritura y ejecucion
        S_IRWXG |
        // OTHERS: Lectura y ejecucion
        S_IROTH | S_IXOTH
        ;

    int error_mkdir = mkdir(configFS->PATH_FCB, mode);
    if (error_mkdir == -1) {
        return -1; 
    }
    return 0;
}

t_config* crear_FCB(char* archivo)
{
    log_debug(logger, "Creando FCB: %s", archivo); 
    
    int fd;
    mode_t mode = 
        // OWNWER: Lectura, escritura y ejecucion
        S_IRUSR | S_IWUSR |
        // GROUP: Lectura, escritura y ejecucion
        S_IRGRP | S_IWGRP |
        // OTHERS: Lectura y ejecucion
        S_IROTH
        ;
    
    char *pathname = string_new();
	
    string_append(&pathname, configFS->PATH_FCB);
	string_append(&pathname, "/");
    string_append(&pathname, archivo);

    // CREO EL FCB: "archivo"
    fd = open(pathname, O_RDWR|O_CREAT|O_TRUNC, mode);
    close(fd);

    // Genero las keys del FCB
    t_config* config_FCB  = config_create(pathname);
    config_set_value(config_FCB, "NOMBRE_ARCHIVO", archivo);
    config_set_value(config_FCB, "TAMANIO_ARCHIVO", "0");
    config_set_value(config_FCB, "PUNTERO_DIRECTO", "-1");
    config_set_value(config_FCB, "PUNTERO_INDIRECTO", "-1");
    config_save(config_FCB);

    return config_FCB;
}

t_config* buscar_FCB(char* archivo) 
{
    char *pathname = string_new();
	
    string_append(&pathname, configFS->PATH_FCB);
	string_append(&pathname, "/");
    string_append(&pathname, archivo);

    return config_create(pathname);
}

t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB)
{
    t_FCB_config* configuracion = malloc(sizeof(t_FCB_config));

	configuracion->NOMBRE_ARCHIVO = config_get_string_value(config_FCB, "NOMBRE_ARCHIVO");
    configuracion->TAMANIO_ARCHIVO = config_get_int_value(config_FCB, "TAMANIO_ARCHIVO");
    configuracion->PUNTERO_DIRECTO = config_get_int_value(config_FCB, "PUNTERO_DIRECTO");
    configuracion->PUNTERO_INDIRECTO = config_get_int_value(config_FCB, "PUNTERO_INDIRECTO");

	return configuracion;
}

t_FCB_config* levantar_FCB(t_config* config)
{
    log_debug(logger, "Levantando FCB"); 
    
    t_FCB_config* config_FCB;

    config_FCB = leerConfiguracion_FCB(config);

    return config_FCB;
}

void actualizar_FCB(t_lista_FCB_config* FCB)
{
    log_debug(logger, "Actualizando FCB");

    config_save(FCB->config);
    char* config_path = string_duplicate(FCB->config->path);

    config_destroy(FCB->config);
    FCB->config = config_create(config_path);
    FCB->FCB_config = levantar_FCB(FCB->config);
}

t_lista_FCB_config* FCB_list_get(char* archivo)
{
    for (int i = 0; i < list_size(l_FCBs_abiertos); i++)
	{
		t_lista_FCB_config *FCB = list_get(l_FCBs_abiertos, i);
		if (strcmp(FCB->nombre_archivo, archivo) == 0)
			return FCB;
	}
	return NULL;
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
