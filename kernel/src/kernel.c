#include "kernel.h"

int main(void) {
	
	//Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);

	log_info(logger, "INICIANDO KERNEL...");

	//Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion();
	
	cargarRecursos();

	crear_hilos_kernel();
}

void crear_hilos_kernel(){
	pthread_t hiloConsola, hiloCPU, hiloFilesystem, hiloMemoria;

	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	pthread_create(&hiloCPU, NULL, (void *)crear_hilo_cpu, NULL);
	pthread_create(&hiloFilesystem, NULL, (void *)crear_hilo_filesystem, NULL);
	pthread_create(&hiloMemoria, NULL, (void *)crear_hilo_memoria, NULL);

	pthread_detach(hiloCPU);
	pthread_detach(hiloFilesystem);
	pthread_detach(hiloMemoria);

	pthread_join(hiloConsola, NULL);
}

void crear_hilo_memoria(){
	//Me conecto a memoria
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA, logger);
	log_info(logger, "Hola, me conecté a memoria");
	while(1){}	
}

void crear_hilo_filesystem(){
	//Me conecto a filesystem
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM, logger);
	log_info(logger, "Hola, me conecté a filesystem");
	while(1){}
}

void crear_hilo_cpu(){
	//Me conecto a cpu
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	log_info(logger, "Hola, me conecté a cpu");
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
	configuracionKernel->ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configuracionKernel->ESTIMACION_INICIAL =config_get_string_value(config, "ESTIMACION_INICIAL");
	configuracionKernel->HRRN_ALFA=config_get_int_value(config, "HRRN_ALFA");
	configuracionKernel->GRADO_MAX_MULTIPROGRAMACION=config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
	configuracionKernel->RECURSOS=config_get_array_value(config, "RECURSOS"); 
	configuracionKernel->INSTANCIAS_RECURSOS=config_get_array_value(config, "INSTANCIAS_RECURSOS");
	
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

void cargarRecursos(){
	
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){
		//log_info(logger, "Tamaño %d\n", size_char_array(configuracionKernel.RECURSOS));
		char *recursoNuevo = configuracionKernel->RECURSOS[i];
		t_recurso *recurso = malloc(sizeof(t_recurso));

		recurso->nombre = recursoNuevo;
		recurso->lista_block = list_create();
		int var_instancias_recursos=atoi(configuracionKernel->INSTANCIAS_RECURSOS[i]);
		recurso->instancias_recursos = var_instancias_recursos;
		//el segundo valor del sem_init es entre cuantos hilos se comparte el semaforo, para id
		//deberia ser entre todos los hilos que va a tener kernel abiertos
		//sem_init(&recurso->contador_bloqueo, 0, var_instancias_recursos);
		//pthread_mutex_init(&recurso->mutex_lista_blocked, NULL);

		//log_info(logger, "recursos %s , %d \n", recurso->recurso, recurso->tiempoEjecucion);
		//agregrar_recurso(recurso);
		log_info(logger,"Se cargo un recurso de nombre: %s y instancias %i", recurso->nombre, recurso->instancias_recursos);
	
	}
}


// void agregrar_recurso(t_recurso *recurso)
// {
// 	pthread_mutex_lock(&mutex_lista_blocked);
// 	list_add(LISTA_BLOCKED, recurso);
// 	pthread_mutex_unlock(&mutex_lista_blocked);
// }