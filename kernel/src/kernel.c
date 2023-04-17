#include "kernel.h"

int main(void) {
	
	//Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);
	
	//Creo el config
	t_config* config = config_create(CONFIG_PATH);

	//Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion();

	//Kernel como cliente
	//Me conecto a memoria
	//int conexion_con_memoria = crear_conexion(configuracionKernel.IP_MEMORIA, configuracionKernel.PUERTO_MEMORIA, logger);
	//Me conecto a filesystem
	//int conexion_con_filesystem = crear_conexion(configuracionKernel.IP_FILESYSTEM, configuracionKernel.PUERTO_FILESYSTEM, logger);
	//int conexion_con_filesystem = crear_conexion("127.0.0.1", configuracionKernel.PUERTO_FILESYSTEM, logger);
	

	crear_hilos_kernel();
	log_info(logger, "kernel termino de esperar");
}

void crear_hilos_kernel(){
	pthread_t hiloConsola, hiloCPU;

	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	pthread_create(&hiloCPU, NULL, (void *)crear_hilo_cpu, NULL);

	pthread_detach(hiloCPU);

	pthread_join(hiloConsola, NULL);
}

void crear_hilo_cpu(){
	//Me conecto a cpu
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	log_info(logger, "Hola, se conecto cpu");
	while(1){}
}

void crear_hilo_consola(){
	int server_fd = iniciar_servidor("127.0.0.1", configuracionKernel->PUERTO_ESCUCHA, logger);
	log_info(logger, "Kernel listo para recibir clientes");

	while (1){
		pthread_t hilo_atender_consola;
		int socketCliente = esperar_cliente(server_fd, logger);//enviarResultado(argumentos->socketCliente, "Quedate tranqui Consola, llego todo lo que mandaste ;)\n");
		pthread_create(&hilo_atender_consola, NULL, (void *)crear_pcb, NULL);
		pthread_detach(hilo_atender_consola);
	}

	log_error(logger, "Muere hilo multiconsolas");
}

void crear_pcb(){
	printf("Hola\n");
}


t_kernel_config *leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_kernel_config *configuracionKernel = malloc(sizeof(t_kernel_config));

	//Leo los datos del config para que kernel funcione como servidor (no tiene IP para funcionar como servido)
	configuracionKernel->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
	
	//Leo los datos del config para que kernel se conecte al resto de modulo
	configuracionKernel->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionKernel->IP_FILESYSTEM = config_get_string_value(config, "IP_FILESYSTEM");
	configuracionKernel->IP_CPU = config_get_string_value(config, "IP_CPU");

	configuracionKernel->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracionKernel->PUERTO_FILESYSTEM = config_get_string_value(config, "PUERTO_FILESYSTEM");
	configuracionKernel->PUERTO_CPU = config_get_string_value(config, "PUERTO_CPU");

	return configuracionKernel;
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
