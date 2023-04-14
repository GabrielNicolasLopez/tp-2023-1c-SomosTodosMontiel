#include "kernel.h"

#define CONFIG_PATH "./cfg/kernel.cfg"
#define LOG_PATH "./cfg/kernel.log"
#define MODULE_NAME "kernel"


int main(void) {
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);
	t_config* config = config_create(CONFIG_PATH);

    char* ip = "127.0.0.1";
    char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

	int server_fd = iniciar_servidor(ip, puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);

			int conexion_memoria = crear_conexion(ip, puerto_memoria, logger);

			enviar_mensaje("Hola, Soy Koronel.", conexion_memoria);
    
    		liberar_conexion(conexion_memoria);

			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			//list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return 0;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return 1;
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
