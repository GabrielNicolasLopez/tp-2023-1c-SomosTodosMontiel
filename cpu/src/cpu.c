#include "cpu.h"

int main(int argc, char ** argv){
    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);
	//logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);

    log_info(logger, "INICIANDO CPU...");

    configuracion_CPU = leerConfiguracion(logger);

    crear_hilos_cpu();
	log_info(logger, "cpu termino de esperar");
}

void crear_hilos_cpu(){
	pthread_t thrKernel;
	if(pthread_create(&thrKernel, NULL, (void *)conectar_kernel, NULL) == 0)
		log_info(logger, "todo bien");
	else
		log_error(logger, "todo mal");
	
	//pthread_detach(thrKernel);
	pthread_join(thrKernel, NULL);
}

void conectar_kernel(){
    int server_fd = iniciar_servidor("127.0.0.1", configuracion_CPU->PUERTO_ESCUCHA, logger);
	log_info(logger, "CPU listo para recibir clientes");

    while (1){
		pthread_t hilo_atender_kernel;
		int socketCliente = esperar_cliente(server_fd, logger);
        pthread_create(&hilo_atender_kernel, NULL, (void *)crear_pcb, NULL);
		pthread_detach(hilo_atender_kernel);
	}

	log_error(logger, "Muere hilo multiconsolas");
}

void crear_pcb(){
	printf("Hola\n");
}

t_configuracion_CPU *leerConfiguracion(t_log* logger){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_configuracion_CPU *configuracion_CPU = malloc(sizeof(t_configuracion_CPU));
	
	configuracion_CPU->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");

	return configuracion_CPU;
}