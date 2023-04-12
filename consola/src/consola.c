#include "consola.h"

#define CONSOLA_CONFIG_PATH "./cfg/consola.cfg"
#define CONSOLA_LOG_PATH "./cfg/consola.log"
#define CONSOLA_MODULE_NAME "consola"

int main(int argc, char* argv[]){

	int conexion;
	char* ip;
	char* puerto;
    t_log *logger;
    t_config* config;

    logger = log_create(CONSOLA_LOG_PATH, CONSOLA_MODULE_NAME, true, LOG_LEVEL_INFO);

    config = config_create(CONSOLA_CONFIG_PATH);
    ip = config_get_string_value(config, "IP");
    puerto = config_get_string_value(config, "PUERTO");

	log_info(logger, ip);
	log_info(logger, puerto);
}
