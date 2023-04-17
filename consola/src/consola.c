#include "consola.h"

int main(int argc, char** argv){
	int conexionKernel;
	
	//Creo logger
	t_log* logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

	log_info(logger, "INICIANDO CONSOLA...");

	//Verifico la correcta inicializacion de la consola
	verificacionDeConfiguracion(argc, logger);

	//Leo la configuracion y la muestro
	t_consola_config consola_config = leerConfiguracion(logger);
	//log_info(logger, "Me conecté a la ip: %s", consola_config.ip);
	//log_info(logger, "Me conecté al puerto: %s", consola_config.puerto);


	//Abro el archivo de instrucciones para sacar las instrucciones
	FILE *archivoInstrucciones = abrirArchivo(argv[2], logger);

	//Creo estructuras
	//t_instrucciones *instrucciones = crearInformacion();

	//Agrego las instrucciones del archivo a la lista
	//agregarInstruccionesDesdeArchivo();

	//Creo el paquete con las instrucciones
	//t_paquete *paqueteInstrucciones = crear_paquete_instrucciones();

	//Consola se conecta a kernel
	conexionKernel = crear_conexion(consola_config.ip, consola_config.puerto, logger);

	log_info(logger, "CONSOLA-KERNEL");
	enviar_mensaje("HOLA SOY LA CONSOLA", conexionKernel);
	
	//Enviamos el paquete
	//enviar_paquete(paqueteInstrucciones, conexionKernel);
	//Borramos el paquete
	//eliminar_paquete(paqueteInstrucciones);
	//Liberamos la memoria de las instrucciones
	//limpiarInformacion(instrucciones);

	log_info(logger, "INSTRUCCIONES ENVIADAS, ESPERANDO...\n");


	/*while(){
		//Esperar mensajes desde kernel, etc	
	}*/

    //Libero la conexion con kernel
    liberar_conexion(conexionKernel);

	//Destruyo el logger
    log_destroy(logger);
}

FILE *abrirArchivo(char *nombreArchivo, t_log* logger){
	if (!nombreArchivo){ //Probar
		log_error(logger, "NOMBRE DE ARCHIVO ERRONEO");
		exit(1);
	}
	return fopen(nombreArchivo, "r");
}

void verificacionDeConfiguracion(int argc, t_log* logger){
	log_info(logger, "Cantidad de parametros: %d", argc);
	if (argc != 3){ //.c, config, pseudocodigo
		log_error(logger, "CANTIDAD DE PARAMETROS INCORRECTA");
		//exit(1);
	}
	else
		log_info(logger, "CONSOLA INICIALIZADA CORRECTAMENTE");
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