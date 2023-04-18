#include "cpu.h"

int main(int argc, char **argv){

	// Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);

	log_info(logger, "INICIANDO CPU...");

	configuracionCPU = leerConfiguracion();

	//Conexion CPU-MEMORIA
	int conexion_con_memoria = crear_conexion(configuracionCPU->IP_MEMORIA, configuracionCPU->PUERTO_MEMORIA, logger);
	log_info(logger, "CPU se conectó a memoria");

	crear_hilos_cpu();
}

void crear_hilos_cpu()
{
	pthread_t thrKernel;

	pthread_create(&thrKernel, NULL, (void *)crear_hilo_kernel, NULL);

	pthread_join(thrKernel, NULL);
}

void crear_hilo_kernel(){
	int server_fd = iniciar_servidor("127.0.0.1", configuracionCPU->PUERTO_ESCUCHA, logger);
	log_info(logger, "CPU listo para recibir clientes");
    int cliente_fd = esperar_cliente(server_fd, logger);
    while(1){}
}

t_cpu_config* leerConfiguracion(){

	// Creo el config para leer IP y PUERTO
	t_config *config = config_create(CONFIG_PATH);

	// Creo el archivo config
	t_cpu_config *configuracionCPU = malloc(sizeof(t_cpu_config));

	configuracionCPU->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
	configuracionCPU->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionCPU->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");

	return configuracionCPU;
}