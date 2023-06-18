#include "memoria.h"

t_memoria_config* configuracion_memoria; 		//variables globales para no vivir pasando datos por parametro.
void* espacioUsuario;
registro_EU* registro_espacioUsuario;

t_list* lista_de_segmentos;
t_segmento* segmento_0;
t_memoria_config* configuracionMemoria;

int conexion_con_kernel;
int conexion_con_memoria;
int conexion_con_cpu;

char* IP_MEMORIA = "127.0.0.1";


int main(int argc, char** argv){

    //Creo logger para info
	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_DEBUG);

    if (argc != NUMBER_OF_ARGS_REQUIRED) {
        log_error(logger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(logger);
        return -1;
    }

	log_debug(logger, "INICIANDO MEMORIA...");

    t_memoria_config *configuracionMemoria = leerConfiguracion();

	int socketEscucha = iniciar_servidor("127.0.0.1", configuracionMemoria->puerto_escucha);

    recibir_conexiones(socketEscucha);

	//Crear hilo para recibir mensajes y ejecutar instrucciones
	while(1){}
	//pthread_join()

	//Finaliza 
}

t_memoria_config* leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* configuracion = config_create(CONFIG_PATH);// nose si poner el path

	//Creo el archivo config
	t_memoria_config* configuracionMemoria = malloc(sizeof(t_memoria_config));

	//Estraer los datos 
	configuracionMemoria->puerto_escucha       = config_get_string_value(configuracion, "PUERTO_ESCUCHA");
	configuracionMemoria->tam_memoria          = config_get_int_value(configuracion, "TAM_MEMORIA");
	configuracionMemoria->tam_segmento_O       = config_get_int_value(configuracion, "TAM_SEGMENTO_0");
	configuracionMemoria->cant_segmentos       = config_get_int_value(configuracion, "CANT_SEGMENTOS");
	configuracionMemoria->retardo_memoria      = config_get_int_value(configuracion, "RETARDO_MEMORIA");
	configuracionMemoria->retardo_compatacion  = config_get_int_value(configuracion, "RETARDO_COMPACTACION");
	configuracionMemoria->algoritmo_asignacion = config_get_string_value(configuracion, "ALGORITMO_ASIGNACION");

	//config_destroy(configuracion);

	return configuracionMemoria;
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
    } else if (handshake == HANDSHAKE_kernel) {
		conexion_con_memoria = socketCliente;
        log_info(logger, "Se acepta conexión de Kernel en socket %d", socketCliente);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
	 } else if (handshake == HANDSHAKE_filesystem) {
		conexion_con_kernel = socketCliente;
        log_info(logger, "Se acepta conexión de Filesystem en socket %d", socketCliente);
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
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

/*void memoria_destroy(t_memoria_config* configuracionMemoria) {
    memoria_config_destroy(configuracionMemoria);
    log_destroy(logger);
}


void memoria_config_destroy(t_memoria_config* configuracionMemoria) {
    free(configuracionMemoria->puerto_escucha);
	free(configuracionMemoria->tam_memoria);
	free(configuracionMemoria->tam_segmento_O);
	free(configuracionMemoria->cant_segmentos);
	free(configuracionMemoria->retardo_memoria);
	free(configuracionMemoria->retardo_compatacion);
    free(configuracionMemoria->algoritmo_asignacion);
	free(configuracionMemoria);
}*/