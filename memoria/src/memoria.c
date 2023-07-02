#include "memoria.h"
#include "funciones.h"


int conexion_con_kernel;
int conexion_con_memoria;
int conexion_con_cpu;

char* IP_MEMORIA = "127.0.0.1";

pthread_t hiloFilesystem, hiloKernel, hiloCPU;
void* espacioUsuario;

t_memoria_config* configuracionMemoria;

//Listas
t_list* listaSegmentos;
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

    configuracionMemoria = leerConfiguracion();
	espacioUsuario = malloc(configuracionMemoria -> tam_memoria );

	segmento_0 = segmentoCrear(0,0,configuracionMemoria->tam_segmento_O);
	hueco_0->base = (segmento_0->base + segmento_0->tamanio);
	hueco_0->tamanio = (configuracionMemoria->tam_memoria - segmento_0->tamanio);  
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
    if (handshake == HANDSHAKE_cpu) {
		conexion_con_cpu = socketCliente;
        log_info(logger, "Se acepta conexión de CPU en socket %d", socketCliente);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
		pthread_create(&hiloCPU, NULL, (void *)hilo_cpu, NULL);
		pthread_detach(hiloCPU);
    } else if (handshake == HANDSHAKE_kernel) {
		conexion_con_memoria = socketCliente;
        log_info(logger, "Se acepta conexión de Kernel en socket %d", socketCliente);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
		pthread_create(&hiloKernel, NULL, (void *)hilo_kernel, NULL);
		pthread_detach(hiloKernel);
	 } else if (handshake == HANDSHAKE_filesystem) {
		conexion_con_kernel = socketCliente;
        log_info(logger, "Se acepta conexión de Filesystem en socket %d", socketCliente);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
		pthread_create(&hiloFilesystem, NULL, (void *)hilo_filesystem, NULL);
		pthread_detach(hiloFilesystem);
    } else {
        log_error(logger, "Error al recibir handshake de cliente: %s", strerror(errno));
        exit(-1);
    }

}

void recibir_conexiones(int socketEscucha){
	//Recibimos a los 3 clientes: kernel, cpu y fs
	for(int i=0; i<3; i++){
    	recibir_conexion(socketEscucha);
	}
}

void hilo_cpu(){
    while(1){
		//Mensajes recibidos de cpu
		//switch
	}
}

void hilo_filesystem(){
    while(1){
		//Mensajes recibidos de fs
		//switch
	}	
}


t_memoria_config* leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* configuracion = config_create(CONFIG_PATH);// nose si poner el path

	//Creo el archivo config
	configuracionMemoria = malloc(sizeof(t_memoria_config));

	//Estraer los datos 
	configuracionMemoria -> puerto_escucha = strdup(config_get_string_value(configuracion, "PUERTO_ESCUCHA"));
	configuracionMemoria -> tam_memoria = config_get_int_value(configuracion, "TAM_MEMORIA");

	configuracionMemoria -> tam_memoria = config_get_int_value(configuracion, "TAM_MEMORIA");
	configuracionMemoria -> tam_segmento_O = config_get_int_value(configuracion, "TAM_SEMENTO_0");
	configuracionMemoria -> cant_segmentos = config_get_int_value(configuracion, "CANT_SEGMENTOS");
	configuracionMemoria -> retardo_memoria = config_get_int_value(configuracion, "RETARDO_MEMORIA");
	configuracionMemoria -> retardo_compatacion = config_get_int_value(configuracion, "RETARDO_COMPACTACION");
	configuracionMemoria -> algoritmo_asignacion = strdup(config_get_int_value(configuracion, "ALGORITMO_ASIGNACION"));

	//Loggeo los datos leidos del config
	//log_info(logger, "Me conecté a la IP: %s", configuracionConsola.ip);
	//log_info(logger, "Me conecté al PUERTO: %s", configuracionConsola.puerto);

	config_destroy(configuracion);

	return configuracionMemoria;
}
