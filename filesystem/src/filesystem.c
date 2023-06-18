#include "filesystem.h"

int conexionConMemoria;
int conexionConKernel;


t_filesystem_config* configuracionFilesystem;

int main(int argc, char **argv){
    
    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);

    log_info(logger, "INICIANDO FS...");

    configuracionFilesystem = leerConfiguracion();

    crear_hilos_filesystem();  
    log_info(logger, "3");
}

void crear_hilos_filesystem(){
	pthread_t hiloKernel, hiloMemoria;

	pthread_create(&hiloKernel, NULL, (void *) hilo_kernel, NULL);
    pthread_create(&hiloMemoria, NULL, (void *) hilo_memoria, NULL);

    pthread_join(hiloKernel, NULL);
    pthread_join(hiloMemoria, NULL);

}


t_filesystem_config *leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config *config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_filesystem_config* configuracionFilesystem = malloc(sizeof(t_filesystem_config));

	//Leo los datos del config (como servidor)
    configuracionFilesystem->PUERTO_ESCUCHA        = config_get_string_value(config, "PUERTO_ESCUCHA");
    //Leo los datos del config (como cliente)
	configuracionFilesystem->IP_MEMORIA            = config_get_string_value(config, "IP_MEMORIA");
	configuracionFilesystem->PUERTO_MEMORIA        = config_get_string_value(config, "PUERTO_MEMORIA");
    configuracionFilesystem->PUERTO_ESCUCHA        = config_get_string_value(config, "PUERTO_ESCUCHA");
    configuracionFilesystem->PATH_SUPERBLOQUE      = config_get_string_value(config, "PATH_SUPERBLOQUE");
    configuracionFilesystem->PATH_BITMAP           = config_get_string_value(config, "PATH_BITMAP");
    configuracionFilesystem->PATH_BLOQUES          = config_get_string_value(config, "PATH_BLOQUES");
    configuracionFilesystem->PATH_FCB              = config_get_string_value(config, "PATH_FCB");
    configuracionFilesystem->RETARDO_ACCESO_BLOQUE = config_get_string_value(config, "RETARDO_ACCESO_BLOQUE");
    
    //config_destroy(config);

	return configuracionFilesystem;
}

void filesystem_destroy(t_filesystem_config* filesystemConfig) {
    filesystem_config_destroy(filesystemConfig);
    log_destroy(logger);
}

void filesystem_config_destroy(t_filesystem_config* filesystemConfig) {
    free(filesystemConfig->PUERTO_ESCUCHA);
    free(filesystemConfig->IP_MEMORIA);
    free(filesystemConfig->PUERTO_MEMORIA);
    free(filesystemConfig->PATH_SUPERBLOQUE);
    free(filesystemConfig->PATH_BITMAP);
    free(filesystemConfig->PATH_BLOQUES);
    free(filesystemConfig->PATH_FCB);
    free(filesystemConfig->RETARDO_ACCESO_BLOQUE);
	free(filesystemConfig);
}