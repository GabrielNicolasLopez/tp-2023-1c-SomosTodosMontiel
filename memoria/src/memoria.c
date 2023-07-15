#include "memoria.h"
#include "funciones.h"
#include "comunicacion.h"

int conexion_con_kernel;
int conexion_con_FileSystem;
int conexion_con_cpu;
int conexion_con_memoria;

char* CONFIG_PATH;
char* IP_MEMORIA = "0.0.0.0";

pthread_t hiloFilesystem, hiloKernel, hiloCPU;
void* espacioUsuario;
t_list* listaSegmentos;
t_memoria_config* configuracionMemoria;
t_list* listaHuecos;

t_segmento* segmento_0;
t_hueco* hueco_0;

int main(int argc, char** argv){

    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_DEBUG);

    if (argc != NUMBER_OF_ARGS_REQUIRED) {
        log_error(logger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(logger);
        return -1;
    }

	log_debug(logger, "INICIANDO MEMORIA...");

	CONFIG_PATH = argv[1];
    //Creo el archivo config
	configuracionMemoria = leerConfiguracion();
	espacioUsuario = malloc(configuracionMemoria -> tam_memoria);

	segmento_0 = malloc(sizeof(t_segmento));
	hueco_0 = malloc(sizeof(t_hueco));

	segmento_0 = segmentoCrear(20000,0,0,configuracionMemoria->tam_segmento_O);
	log_info(logger,"Se creo el segmento 0 global Tam: %d",segmento_0->tamanio);
	hueco_0->base = (segmento_0->base + segmento_0->tamanio);
	hueco_0->tamanio = (configuracionMemoria->tam_memoria - segmento_0->tamanio);  
	
	listaSegmentos = list_create();
	listaHuecos = list_create();
	
	list_add(listaSegmentos,segmento_0);
	list_add(listaHuecos,hueco_0);

	int socketEscucha = iniciar_servidor(IP_MEMORIA, configuracionMemoria->puerto_escucha);

    recibir_conexiones(socketEscucha);

	//Espera la finalizacion de los hilos
	pthread_join(hiloFilesystem, NULL);
	pthread_join(hiloKernel, NULL);
	pthread_join(hiloCPU, NULL);
}

void recibir_conexion(int socketEscucha) {
	log_debug(logger, "Esperando cliente...");
	int socketCliente = esperar_cliente(socketEscucha);
    uint8_t handshake = stream_recv_header(socketCliente);
    stream_recv_empty_buffer(socketCliente);
	switch(handshake){
		case HANDSHAKE_cpu:
			conexion_con_cpu = socketCliente;
			log_info(logger, "Se acepta conexión de CPU en socket %d", conexion_con_cpu);
			stream_send_empty_buffer(conexion_con_cpu, HANDSHAKE_ok_continue);
			pthread_create(&hiloCPU, NULL, (void *)hilo_cpu, NULL);
			break;
		case HANDSHAKE_kernel:
			conexion_con_kernel = socketCliente;
			log_info(logger, "Se acepta conexión de Kernel en socket %d", conexion_con_kernel);
			stream_send_empty_buffer(conexion_con_kernel, HANDSHAKE_ok_continue);
			pthread_create(&hiloKernel, NULL, (void *)hilo_kernel_m, NULL);
			break;
		case HANDSHAKE_filesystem:
			conexion_con_FileSystem = socketCliente;
			log_info(logger, "Se acepta conexión de Filesystem en socket %d", conexion_con_FileSystem);
			stream_send_empty_buffer(conexion_con_FileSystem, HANDSHAKE_ok_continue);
			pthread_create(&hiloFilesystem, NULL, (void *)hilo_filesystem, NULL);
			break;
		default:
			log_error(logger, "Error al recibir handshake de cliente: %s", strerror(errno));
			exit(-1);
			break;
	}


}

void recibir_conexiones(int socketEscucha){
	//Recibimos a los 3 clientes: kernel, cpu y fs
	for(int i=0; i<3; i++){
    	recibir_conexion(socketEscucha);
	}
}


t_memoria_config* leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* configuracion = config_create(CONFIG_PATH);// nose si poner el path

	//Creo el archivo config
	configuracionMemoria = malloc(sizeof(t_memoria_config));

	//Estraer los datos 
	configuracionMemoria -> puerto_escucha       = config_get_string_value(configuracion, "PUERTO_ESCUCHA");
	configuracionMemoria -> tam_memoria          = config_get_int_value(configuracion, "TAM_MEMORIA");
	configuracionMemoria -> cant_segmentos       = config_get_int_value(configuracion, "CANT_SEGMENTOS");
	configuracionMemoria -> retardo_memoria      = config_get_int_value(configuracion, "RETARDO_MEMORIA");
	configuracionMemoria -> retardo_compatacion  = config_get_int_value(configuracion, "RETARDO_COMPACTACION");
	configuracionMemoria -> algoritmo_asignacion = config_get_string_value(configuracion, "ALGORITMO_ASIGNACION");
	configuracionMemoria -> tam_segmento_O       = config_get_int_value(configuracion, "TAM_SEGMENTO_0");

	/* log_error(logger, "tam_segm0: %d", configuracionMemoria->tam_segmento_O); */

	return configuracionMemoria;
}



