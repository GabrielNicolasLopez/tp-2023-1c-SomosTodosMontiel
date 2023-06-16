#include "filesystem.h"

t_filesystem_config* configuracionFS;

int main(int argc, char ** argv){
    if (argc != 2) {
        fprintf(stderr, "Se esperaba: %s [CONFIG_PATH] - Abortando...", argv[0]);
        exit(EXIT_FAILURE);
    }
	char *CONFIG_PATH = argv[1];

	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);
    if (logger == NULL) {
        fprintf(stderr, "Error al abrir el logger, abortando...");
        exit(EXIT_FAILURE);
    }
    log_info(logger, "INICIANDO FILESYSTEM...");

    // CONFIG
    t_config *config = config_create(CONFIG_PATH);
    configuracionFS = leerConfiguracion(config);

    // CREACION DE HILOS
    crear_hilos_filesystem();


    // ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
	config_destroy(config);
	free(configuracionFS);
}


// CREACION DE HILOS
void crear_hilos_filesystem()
{
	pthread_t hiloMemoria, hiloKernel;
	
    pthread_create(&hiloMemoria, NULL, &crear_hilo_memoria, NULL);
    pthread_create(&hiloKernel, NULL, &crear_hilo_kernel, NULL);



    int* hiloMemoria_return;
    pthread_join(hiloMemoria, (void**) &hiloMemoria_return);
    if (*hiloMemoria_return) {
        log_error(logger, "Error en HILO_MEMORIA");
    }
    
    int* hiloKernel_return;
    pthread_join(hiloKernel, (void**) &hiloKernel_return);
    if (!*hiloKernel_return) {
        log_error(logger, "Error en HILO_KERNEL");
    }
    
    
}

// CONFIGURACION
t_filesystem_config *leerConfiguracion(t_config* config)
{
    t_filesystem_config* configuracion = malloc(sizeof(t_filesystem_config));

	configuracion->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracion->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    configuracion->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    configuracion->PATH_SUPERBLOQUE = config_get_string_value(config, "PATH_SUPERBLOQUE");
    configuracion->PATH_BITMAP = config_get_string_value(config, "PATH_BITMAP");
    configuracion->PATH_BLOQUES = config_get_string_value(config, "PATH_BLOQUES");
    configuracion->PATH_FCB = config_get_string_value(config, "PATH_FCB");
    configuracion->RETARDO_ACCESO_BLOQUE = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

	return configuracion;
}
