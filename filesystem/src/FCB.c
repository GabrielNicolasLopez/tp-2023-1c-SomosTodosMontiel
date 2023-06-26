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

t_FCB_config* levantar_FCB(t_config* t_config_FCB)
{
    log_debug(logger, "Levantando FCB"); 
    
    t_FCB_config* config_FCB = malloc(sizeof(t_FCB_config));

    config_FCB = leerConfiguracion_FCB(t_config_FCB);

    return config_FCB;
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
    // DIFERENCIA DE TAMAÑO DEL ARCHIVO

    // CONSTANTES DE MI FS:
        // TAMAÑO DE BLOQUE DE DATOS
        uint32_t tam_bloque = config_SupBloque->BLOCK_SIZE;
        // TAMAÑO DEL PUNTERO
        uint32_t tam_puntero = sizeof(FCB->FCB_config->PUNTERO_INDIRECTO);
        // CANTIDAD DE PUNTEROS POR BLOQUE
        uint32_t cant_punt_x_bloque = tam_bloque / tam_puntero;
        // TAMAÑO MAXIMO REAL DEL ARCHIVO
        uint32_t tam_max_arch = minimum(tam_bloque * (1 + 1 * cant_punt_x_bloque), tam_bloque * config_SupBloque->BLOCK_COUNT);

    // CANTIDAD DE BLOQUES YA OCUPADOS
    uint32_t bloques_ya_ocupados = ceil((double) tam_ant_arch / tam_bloque);
    // CANTIDAD DE BLOQUES TOTALES A OCUPAR
    uint32_t bloques_totales_a_ocupar = ceil((double) tam_nuevo_arch / tam_bloque);
    // CANTIDAD DE BLOQUES NUEVOS A OCUPAR
    uint32_t bloques_nuevos_a_ocupar = bloques_totales_a_ocupar - bloques_ya_ocupados;
    
    config_set_value(FCB->config, "TAMANIO_ARCHIVO", string_itoa(tam_nuevo_arch));
    
    if (tam_nuevo_arch > tam_max_arch) {
        log_error(logger, "Tamaño de truncamiento invalido, archivo: %s", FCB->nombre_archivo); 
        return -1;
    }
    // Casos dondo no hay asignado un puntero directo
    if (tam_ant_arch == 0) {
        // Asigno puntero directo
        config_set_value(FCB->config, "PUNTERO_DIRECTO", string_itoa(get_free_block()));
    } 
    if (bloques_totales_a_ocupar > 1) {
        // Casos dondo no hay asignado un puntero indirecto
        if (tam_ant_arch <= tam_bloque) {
            uint32_t bloque_punteros = get_free_block();
            // Asigno puntero indirecto
            config_set_value(FCB->config, "PUNTERO_INDIRECTO", string_itoa(bloque_punteros));
        }
        
        if (bloques_nuevos_a_ocupar > 0) {
            // Asigno la cantidad de punteros a bloques de datos que necesito
            uint32_t bloque_a_ocupar;
            for(int i; i < bloques_nuevos_a_ocupar; i++) {
                bloque_a_ocupar = get_free_block();
                
                // continuar ...
            }
        }
    }
    
    
    config_save(config);
    msync(p_bitmap, stats_fd_bitmap.st_size, MS_SYNC);

    return 0;
}


uint32_t minimum(uint32_t x, uint32_t y)
{
    if (x > y)
        return y;
    return x;
}