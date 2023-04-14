#include "consola.h"

int main(void)
{
	int conexion;
	
	//Creo logger para info
	t_log* logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

	//Leo la configuracion y la muestro
	t_consola_config consola_config = leerConfiguracion(logger);
	log_info(logger, "Me conecté a la ip: %s", consola_config.ip);
	log_info(logger, "Me conecté al puerto: %s", consola_config.puerto);

	//Consola se conecta a kernel
	conexion = crear_conexion(consola_config.ip, consola_config.puerto, logger);

	enviar_mensaje("HOLA SOY LA CONSOLA", conexion);
	
    //Libera la conexion
    liberar_conexion(conexion);
	//Destruye el logger
    log_destroy(logger);
}

t_consola_config leerConfiguracion(t_log* logger){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_consola_config configuracionConsola;

	//Leo los datos del config
	configuracionConsola.ip = config_get_string_value(config, "IP");
	configuracionConsola.puerto = config_get_string_value(config, "PUERTO");
	
	//Loggeo los datos leidos del config
	//log_info(logger, "Me conecté a la IP: %s", configuracionConsola.ip);
	//log_info(logger, "Me conecté al PUERTO: %s", configuracionConsola.puerto);

	return configuracionConsola;
}