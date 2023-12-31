#include "kernel.h"

int main(int argc, char** argv){

	// Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);

	if (argc != NUMBER_OF_ARGS_REQUIRED) {
        log_debug(logger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(logger);
        return -1;
    }

	char *CONFIG_PATH = argv[1];

	log_debug(logger, "INICIANDO KERNEL...");

	// Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion(CONFIG_PATH);

	// Inicializo las listas y semaforos
	iniciar_listas_y_semaforos();

	//Inicializo las estructuras con sus valores de cada recurso
	cargarRecursos();

	//Creo los hilos para que controlen todo 
	crear_hilos_kernel();

	//Liberar recursos
	liberar_listas_y_semaforos();

	//Aviso de finalizacion de kernel
	log_debug(logger, "KERNEL TERMINO DE EJECUTAR...");
}

t_kernel_config *leerConfiguracion(char* path){

	// Creo el config para leer IP y PUERTO
	config = config_create(path);

	// Creo el archivo config
	t_kernel_config *configuracionKernel = malloc(sizeof(t_kernel_config));

	// Leo los datos del config para que kernel funcione como servidor (no tiene IP para funcionar como servido)
	configuracionKernel->PUERTO_ESCUCHA              = config_get_string_value(config, "PUERTO_ESCUCHA");
	configuracionKernel->ALGORITMO_PLANIFICACION     = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configuracionKernel->ESTIMACION_INICIAL          = config_get_int_value(config, "ESTIMACION_INICIAL");
	configuracionKernel->HRRN_ALFA                   = config_get_double_value(config, "HRRN_ALFA");
	configuracionKernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");

	configuracionKernel->RECURSOS                    = config_get_array_value(config, "RECURSOS");
	configuracionKernel->INSTANCIAS_RECURSOS         = config_get_array_value(config, "INSTANCIAS_RECURSOS");

	// Leo los datos del config para que kernel se conecte al resto de modulo
	configuracionKernel->IP_MEMORIA                  = config_get_string_value(config, "IP_MEMORIA");
	configuracionKernel->IP_FILESYSTEM               = config_get_string_value(config, "IP_FILESYSTEM");
	configuracionKernel->IP_CPU                      = config_get_string_value(config, "IP_CPU");

	configuracionKernel->PUERTO_MEMORIA              = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracionKernel->PUERTO_FILESYSTEM           = config_get_string_value(config, "PUERTO_FILESYSTEM");
	configuracionKernel->PUERTO_CPU                  = config_get_string_value(config, "PUERTO_CPU");

	return configuracionKernel;
}

void iniciar_listas_y_semaforos(){

	LISTA_NEW = list_create();
	LISTA_READY = list_create();
	LISTA_EXEC = list_create();
	LISTA_BLOCKED = list_create();
	LISTA_EXIT  = list_create();
	LISTA_TGAA = list_create();
	LISTA_TABLA_SEGMENTOS = list_create();
	LISTA_PCBS_EN_RAM = list_create();

	sem_init(&CantPCBNew, 0, 0);
	sem_init(&CPUVacia, 0, 1);
	sem_init(&pasar_pcb_a_CPU, 0, 0);
	sem_init(&multiprogramacion, 0, configuracionKernel->GRADO_MAX_MULTIPROGRAMACION);	
	sem_init(&esPosibleCompactar, 0, 1); //CReo que va en 0
	sem_init(&FS_Continue, 0, 0);	

}

void crear_hilos_kernel(){
	pthread_t hiloConsola, hiloGeneral, hiloPlaniCortoPlazo, hiloPlaniLargoPlazo;

	//Hilos de modulos
	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	pthread_create(&hiloGeneral, NULL, (void *)hilo_general, NULL);
	
	pthread_detach(hiloGeneral);

	//Hilos de planificadores
	pthread_create(&hiloPlaniCortoPlazo, NULL, (void *)planiCortoPlazo, NULL);
	pthread_create(&hiloPlaniLargoPlazo, NULL, (void *)planiLargoPlazo, NULL);

	pthread_detach(hiloPlaniCortoPlazo);
	pthread_detach(hiloPlaniLargoPlazo);

	//Esperamos a que finalice el hilo de consola
	pthread_join(hiloConsola, NULL);
}

void liberar_listas_y_semaforos(){}

void cargarRecursos(){
	//Creo la estructura de lista para guardar todos los recursos con sus atributos
	lista_de_recursos = list_create();

	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){

		t_recurso *recurso = malloc(sizeof(t_recurso));
		//Agrego el nombre del recurso
		recurso->nombre = configuracionKernel->RECURSOS[i];
		//Cargo la cantidad de recursos
		recurso->instancias_recursos = atoi(configuracionKernel->INSTANCIAS_RECURSOS[i]);
		//Creo la lista de bloqueados para que los procesos se encolen
		recurso->lista_block = list_create();
		//Inicializo el mutex
		pthread_mutex_init(&recurso->mutex_lista_blocked, NULL);

		//Agrego el recurso a la lista
		list_add(lista_de_recursos, recurso);

		log_debug(logger, "Se cargo: %10s\t x%d", recurso->nombre, recurso->instancias_recursos);
	}
}
