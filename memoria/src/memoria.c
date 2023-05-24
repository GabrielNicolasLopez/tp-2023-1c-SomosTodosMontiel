#include "memoria.h"

t_memoria_config* configuracion_memoria; 		//variables globales para no vivir pasando datos por parametro.
void* espacioUsuario;
registro_EU* registro_espacioUsuario;




int main(){

    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

    log_info(logger, "INICIANDO MEMORIA...");

    configuracionMemoria = leerConfiguracion(logger);

    /*int server_fd = iniciar_servidor("127.0.0.1", configuracionMemoria->PUERTO_ESCUCHA, logger);
    log_info(logger, "Memoria lista para recibir a los modulos");
    int cliente_fd = esperar_cliente(server_fd, logger);*/

    crear_hilos_memoria();
    log_info(logger, "Hola");
}

void crear_hilos_memoria()
{
	pthread_t hiloKernel, hiloCPU, hiloFilesystem;

	pthread_create(&hiloKernel, NULL, (void *)iniciar_servidor_hacia_kernel, NULL);
	pthread_create(&hiloCPU, NULL, (void *)iniciar_servidor_hacia_cpu, NULL);
    pthread_create(&hiloFilesystem, NULL, (void *)iniciar_servidor_hacia_filesystem, NULL);

	pthread_detach(hiloKernel);
    pthread_detach(hiloFilesystem);
	pthread_join(hiloCPU, NULL);
}

void iniciar_servidor_hacia_kernel(){
    //Inicio el servidor para kernel
    int server_fd = iniciar_servidor(IP_MEMORIA, configuracionMemoria->PUERTO_ESCUCHA_KERNEL, logger);
	log_info(logger, "MEMORIA listo para recibir al kernel");
	socketKernel = esperar_cliente(server_fd, logger);

    while(1){}
}

void iniciar_servidor_hacia_cpu(){
    //Inicio el servidor para CPU
    int server_fd = iniciar_servidor(IP_MEMORIA, configuracionMemoria->PUERTO_ESCUCHA_CPU, logger);
	log_info(logger, "MEMORIA listo para recibir al CPU");
	socketCPU = esperar_cliente(server_fd, logger);

    while(1){}
}

void iniciar_servidor_hacia_filesystem(){
    //Inicio el servidor para filesystem
    int server_fd = iniciar_servidor(IP_MEMORIA, configuracionMemoria->PUERTO_ESCUCHA_FILESYSTEM, logger);
	log_info(logger, "MEMORIA listo para recibir al FILESYSTEM");
	socketFilesystem = esperar_cliente(server_fd, logger);

    while(1){}
}


t_memoria_config* leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* configuracion = config_create(CONFIG_PATH);// nose si poner el path

	//Creo el archivo config
	t_memoria_config* configuracionMemoria = malloc(sizeof(t_memoria_config));

	//Estraer los datos 
	configuracionMemoria -> PUERTO_ESCUCHA = strdup(config_get_string_value(configuracion, "PUERTO_ESCUCHA"));
	configuracionMemoria -> tam_memoria = config_get_int_value(configuracion, "TAM_MEMORIA");

	configuracionMemoria -> tam_memoria = config_get_int_value(configuracion, "TAM_MEMORIA");
	configuracionMemoria -> tam_segmento_O = config_get_int_value(configuracion, "TAM_SEMENTO_0");
	configuracionMemoria -> cant_segmentos = config_get_int_value(configuracion, "CANT_SEGMENTOS");
	configuracionMemoria -> retardo_memoria = config_get_int_value(configuracion, "RETARDO_MEMORIA");
	configuracionMemoria -> retardo_compatacion = config_get_int_value(configuracion, "RETARDO_COMPACTACION");
	configuracionMemoria -> algoritmo_asignacion = strdup(config_get_int_value(configuracion, "ALGORITMO_ASIGNACION"));



	//Leo los datos del config (memoria no tiene IP en el archivo de config)
	//configuracionConsola.ip = config_get_string_value(config, "IP");
	configuracionMemoria->PUERTO_ESCUCHA_KERNEL = config_get_string_value(configuracion, "PUERTO_ESCUCHA_KERNEL");
    configuracionMemoria->PUERTO_ESCUCHA_CPU = config_get_string_value(configuracion, "PUERTO_ESCUCHA_CPU");
    configuracionMemoria->PUERTO_ESCUCHA_FILESYSTEM = config_get_string_value(configuracion, "PUERTO_ESCUCHA_FILESYSTEM");
	
	//Loggeo los datos leidos del config
	//log_info(logger, "Me conecté a la IP: %s", configuracionConsola.ip);
	//log_info(logger, "Me conecté al PUERTO: %s", configuracionConsola.puerto);

	config_destroy(configuracion);

	return configuracionMemoria;
}


void configurar_memoria(char* path){

	leerConfiguracion(path);

	espacioUsuario = malloc(configuracion_memoria -> tam_memoria );

	//Me falta ver como hacer lo de espacio usuario 
	

}