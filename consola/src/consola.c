#include "consola.h"

#define CONFIG_PATH "./cfg/consola.cfg"
#define LOG_PATH "./cfg/consola.log"
#define MODULE_NAME "consola"

int main(void)
{
	t_log* logger;
	t_config* config = config_create(CONFIG_PATH);

	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	char* ip = config_get_string_value(config, "IP");
	char* puerto_kernel = config_get_string_value(config, "PUERTO");

	int conexion_con_kernel = crear_conexion(ip, puerto_kernel, logger);

	enviar_mensaje("HOLA SOY LA CONSOLA MANDA SALUDO A MEMORIA", conexion_con_kernel);
    
    liberar_conexion(conexion_con_kernel);

    log_destroy(logger);
}