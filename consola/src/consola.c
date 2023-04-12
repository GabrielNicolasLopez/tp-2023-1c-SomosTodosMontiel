#include "consola.h"

#define CONFIG_PATH "./cfg/consola.cfg"
#define LOG_PATH "./cfg/consola.log"
#define MODULE_NAME "consola"

int main(void)
{
	int conexion;
	t_log* logger;

	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	char* ip = "127.0.0.1";
	char* puerto = "4444";

	conexion = crear_conexion(ip, puerto, logger);

	enviar_mensaje("HOLA SOY LA CONSOLA", conexion);
    
    liberar_conexion(conexion);

    log_destroy(logger);
}