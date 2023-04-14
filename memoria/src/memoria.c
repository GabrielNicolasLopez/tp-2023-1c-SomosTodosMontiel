#include "memoria.h"

int main(){

    //Creo logger para info
	t_log* logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

    t_memoria_config configuracionMemoria = leerConfiguracion(logger);

    int server_fd = iniciar_servidor("127.0.0.1", configuracionMemoria.PUERTO_ESCUCHA, logger);
    log_info(logger, "Memoria lista para recibir a los modulos");
    int cliente_fd = esperar_cliente(server_fd, logger);

    while (1) {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op) {
        case MENSAJE:
            recibir_mensaje(cliente_fd);
            break;
        case -1:
            log_error(logger, "Un cliente se desconecto.");
            break;
        default:
            log_warning(logger,"Operacion desconocida. No quieras destruir tu sistema operativo");
            break;
        }
    }

    log_destroy(logger); 
}

t_memoria_config leerConfiguracion(t_log* logger){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_memoria_config configuracionMemoria;

	//Leo los datos del config (memoria no tiene IP en el archivo de config)
	//configuracionConsola.ip = config_get_string_value(config, "IP");
	configuracionMemoria.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
	
	//Loggeo los datos leidos del config
	//log_info(logger, "Me conecté a la IP: %s", configuracionConsola.ip);
	//log_info(logger, "Me conecté al PUERTO: %s", configuracionConsola.puerto);

	return configuracionMemoria;
}