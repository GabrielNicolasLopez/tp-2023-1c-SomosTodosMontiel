#include "kernel.h"

#define CONFIG_PATH "./cfg/kernel.cfg"
#define LOG_PATH "./cfg/kernel.log"
#define MODULE_NAME "kernel"


int main(void) {
	
	//Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);
	
	//Creo el config
	t_config* config = config_create(CONFIG_PATH);

	//Leo la configuracion de kernel
	t_kernel_config configuracionKernel = leerConfiguracion();

	//Kernel como cliente
	//Me conecto a memoria
	int conexion_con_memoria = crear_conexion(configuracionKernel.IP_MEMORIA, configuracionKernel.PUERTO_MEMORIA, logger);
	//Me conecto a filesystem
	//int conexion_con_filesystem = crear_conexion(configuracionKernel.IP_FILESYSTEM, configuracionKernel.PUERTO_FILESYSTEM, logger);
	int conexion_con_filesystem = crear_conexion("127.0.0.1", configuracionKernel.PUERTO_FILESYSTEM, logger);
	//Me conecto a cpu
	//int conexion_con_cpu = crear_conexion(configuracionKernel.IP_CPU, configuracionKernel.PUERTO_CPU, logger);

	//Inicio kernel como servidor
	int server_fd = iniciar_servidor("127.0.0.3", configuracionKernel.PUERTO_ESCUCHA, logger);
	log_info(logger, "Kernel listo para recibir consolas");
	int cliente_fd = esperar_cliente(server_fd, logger);


	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			//list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return 1;
}

t_kernel_config leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_kernel_config configuracionKernel;

	//Leo los datos del config para que kernel funcione como servidor (no tiene IP para funcionar como servido)
	configuracionKernel.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
	
	//Leo los datos del config para que kernel se conecte al resto de modulo
	configuracionKernel.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionKernel.IP_FILESYSTEM = config_get_string_value(config, "IP_FILESYSTEM");
	configuracionKernel.IP_CPU = config_get_string_value(config, "IP_CPU");

	configuracionKernel.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracionKernel.PUERTO_FILESYSTEM = config_get_string_value(config, "PUERTO_FILESYSTEM");
	configuracionKernel.PUERTO_CPU = config_get_string_value(config, "PUERTO_CPU");

	return configuracionKernel;
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
