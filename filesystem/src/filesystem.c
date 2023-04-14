#include "filesystem.h"

int main(int argc, char ** argv){
        //Creo logger para info
	t_log* logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

   t_filesystem_config configuracionFilesystem = leerConfiguracion();

    //Filesystem como cliente
    //Me conecto a memoria
//    int conexion_con_memoria = crear_conexion(configuracionFilesystem.IP_MEMORIA, configuracionFilesystem.PUERTO_MEMORIA, logger);
    int conexion_con_memoria = crear_conexion("127.0.0.1", configuracionFilesystem.PUERTO_MEMORIA, logger);

    //Filesystem como servidor
    int server_fd = iniciar_servidor("127.0.0.1", configuracionFilesystem.PUERTO_ESCUCHA, logger);
    log_info(logger, "Filesystem listo para recibir a Kernel");
    int cliente_fd = esperar_cliente(server_fd, logger);

    while (1) {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op) {
        case MENSAJE:
            recibir_mensaje(cliente_fd);
            break;
        case -1:
            log_error(logger, "Kernel se desconecto.");
            break;
        default:
            log_warning(logger,"Operacion desconocida. No quieras destruir tu sistema operativo");
            break;
        }
    }

    log_destroy(logger);    
}

t_filesystem_config leerConfiguracion(){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_filesystem_config configuracionFilesystem;

	//Leo los datos del config (como servidor)
    configuracionFilesystem.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    //Leo los datos del config (como cliente)
	configuracionFilesystem.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionFilesystem.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
	
	//Loggeo los datos leidos del config
	//log_info(logger, "Me conecté a la IP: %s", configuracionConsola.ip);
	//log_info(logger, "Me conecté al PUERTO: %s", configuracionConsola.puerto);

	return configuracionFilesystem;
}