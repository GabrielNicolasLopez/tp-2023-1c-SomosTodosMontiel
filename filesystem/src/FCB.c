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

bool existe_FCB(char* nombre_FCB)
{
    char *pathname = string_new();
	
    string_append(&pathname, configFS->PATH_FCB);
	string_append(&pathname, "/");
    string_append(&pathname, nombre_FCB);

    bool existe = existe_archivo(pathname);
    free(pathname);
    
    return existe;
}