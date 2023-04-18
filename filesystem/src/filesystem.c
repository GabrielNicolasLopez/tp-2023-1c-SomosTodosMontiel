#include "filesystem.h"

int main(int argc, char ** argv){
    
    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);

    log_info(logger, "INICIANDO FILESYSTEM...");

    configuracionFilesystem = leerConfiguracion();

    //Me conecto a memoria
    int conexion_con_memoria = crear_conexion(configuracionFilesystem->IP_MEMORIA, configuracionFilesystem->PUERTO_MEMORIA, logger);
    log_info(logger, "FL se conectó a memoria");
    
    crear_hilos_filesystem();  
    log_info(logger, "3");
}

void crear_hilos_filesystem(){
	pthread_t hiloKernel;

	pthread_create(&hiloKernel, NULL, (void *)crear_hilo_kernel, NULL);

	pthread_join(hiloKernel, NULL);
}

void crear_hilo_kernel(){
    //Abro el server para kernel
    int server_fd = iniciar_servidor("127.0.0.1", configuracionFilesystem->PUERTO_ESCUCHA, logger);
    log_info(logger, "Filesystem listo para recibir a Kernel");
    int cliente_fd = esperar_cliente(server_fd, logger);
    while(1){}
}

/*void crear_hilo_memoria(){
    //Me conecto a memoria
    int conexion_con_memoria = crear_conexion(configuracionFilesystem->IP_MEMORIA, configuracionFilesystem->PUERTO_MEMORIA, logger);
    log_info(logger, "Me conecté a memoria");
}*/



t_filesystem_config *leerConfiguracion(){

    log_info(logger, "Hola FL 1");

	//Creo el config para leer IP y PUERTO
	t_config *config = config_create(CONFIG_PATH);

    log_info(logger, "Hola FL 2");

	//Creo el archivo config
	t_filesystem_config* configuracionFilesystem = malloc(sizeof(t_filesystem_config));

	//Leo los datos del config (como servidor)
    configuracionFilesystem->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    //Leo los datos del config (como cliente)
	configuracionFilesystem->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionFilesystem->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");

	return configuracionFilesystem;
}