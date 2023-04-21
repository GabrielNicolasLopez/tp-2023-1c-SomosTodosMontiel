#include "consola.h"

int main(int argc, char** argv){
	int conexionKernel;
	
	//Creo logger
	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

	log_info(logger, "INICIANDO CONSOLA...");

	//Verifico la correcta inicializacion de la consola
	verificacionDeConfiguracion(argc, logger);

	//Leo la configuracion y la muestro
	t_consola_config consola_config = leerConfiguracion(logger);

	//Abro el archivo de instrucciones para sacar las instrucciones
	//FILE *archivoInstrucciones = abrirArchivo(argv[2], logger);

	/*t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
	instrucciones->listaInstrucciones = list_create();

	//Agrego las instrucciones del archivo a la lista
	agregarInstruccionesDesdeArchivo(instrucciones, archivoInstrucciones);

	//Creo el paquete con las instrucciones
	//t_paquete *paqueteInstrucciones = crear_paquete_instrucciones();
	*/
	//Consola se conecta a kernel
	conexionKernel = crear_conexion(consola_config.ip, consola_config.puerto, logger);

	//log_info(logger, "CONSOLA-KERNEL");
	enviar_mensaje("HOLA SOY LA CONSOLA", conexionKernel);
	
	//Enviamos el paquete
	//enviar_paquete(paqueteInstrucciones, conexionKernel);
	//Borramos el paquete
	//eliminar_paquete(paqueteInstrucciones);
	//Liberamos la memoria de las instrucciones
	//limpiarInformacion(instrucciones);

	//log_info(logger, "INSTRUCCIONES ENVIADAS, ESPERANDO...\n");


	/*while(){
		//Esperar mensajes desde kernel, etc	
	}*/

    //Libero la conexion con kernel
    liberar_conexion(conexionKernel);

	//Destruyo el logger
    log_destroy(logger);
}


void agregarInstruccionesDesdeArchivo(t_instrucciones *instrucciones, FILE* archivoInstrucciones){
	if (archivoInstrucciones == NULL){
		log_error(logger, "NO SE PUEDE ABRIR EL ARCHIVO DE INSTRUCCIONES, ABORTANDO CONSOLA...");
		exit(1);
	}
	log_info(logger, "ARCHIVO DE INSTRUCCIONES ABIERTO, LEYENDO INSTRUCCIONES...");
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	while (fgets(buffer, 256, archivoInstrucciones) != NULL){ //Mientras pueda leer del archivo
		//Una vez que lee una linea, crea una lista con cada una de las palabras que se generan luego haberlas separado por un espacio 
		char **palabras = string_split(buffer, " "); 
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));

		if (strcmp(palabras[0], "SET") == 0){
			instruccion->tipo = SET;
			instruccion->registros[0] = devolverRegistro(palabras[1]); //Registro
			char *contenidoRegistro = string_new();
			string_append(&contenidoRegistro, palabras[2]);
			instruccion->cadenaRegistro = contenidoRegistro;
			//Anular el resto de parametros no usados
			instruccion-> paramIntA = -1;
			instruccion-> paramIntB = -1;
			instruccion-> recurso = "";
			instruccion->nombreArchivo = "";
			instruccion->registros[1] = -1;
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "MOV_IN") == 0){
			instruccion->tipo = MOV_IN;
			instruccion->registros[0] = devolverRegistro(palabras[1]); //Registro
			instruccion->paramIntA = atoi(palabras[2]); //Direccion logica
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "MOV_OUT") == 0){
			//MOV_OUT 120 AX
			instruccion->tipo = MOV_OUT;
			instruccion->paramIntA = atoi(palabras[1]); //Direccion logica
			instruccion->registros[0] = devolverRegistro(palabras[2]); //Registro
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->recurso = "";
			instruccion->cadenaRegistro = "";
			instruccion->nombreArchivo = "";
			instruccion->registros[1] = -1;
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "I/O") == 0){
			instruccion->tipo = IO;
			instruccion->paramIntA = atoi(palabras[1]); //Tiempo
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "F_OPEN") == 0){
			instruccion->tipo = F_OPEN;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1;
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "F_CLOSE") == 0)
		{
			instruccion->tipo = F_CLOSE;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1;
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "F_SEEK") == 0){
			instruccion->tipo = F_SEEK;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			instruccion->paramIntA = atoi(palabras[2]); //Posicion del archivo
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "F_READ") == 0)
		{
			instruccion->tipo = F_READ;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			instruccion->paramIntA = -1; //Direccion logica
			instruccion->paramIntB = -1; //Cantidad de bytes
			//Anular el resto de parametros no usados
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "F_WRITE") == 0)
		{
			instruccion->tipo = F_WRITE;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			instruccion->paramIntA = atoi(palabras[2]); //Direccion logica
			instruccion->paramIntB = atoi(palabras[3]); //Cantidad de bytes
			//Anular el resto de parametros no usados
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
			free(palabras[3]);
		}
		else if (strcmp(palabras[0], "F_TRUNCATE") == 0){
			instruccion->tipo = F_TRUNCATE;
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			instruccion->paramIntA = atoi(palabras[2]); //Tamaño del archivo
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "WAIT") == 0){
			instruccion->tipo = WAIT;
			//Arreglar
			char * recurso = string_new();
			string_append(&recurso, palabras[1]);
			instruccion->cadenaRegistro = recurso;
			//Arreglar
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1; 
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "SIGNAL") == 0){
			instruccion->tipo = SIGNAL;

			char *recurso = string_new();
			string_append(&recurso, palabras[1]);
			instruccion->recurso = recurso;

			//strcpy(instruccion->recurso, palabras[1]);
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1; 
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "CREATE_SEGMENT") == 0)
		{
			instruccion->tipo = CREATE_SEGMENT;
			instruccion->paramIntA = atoi(palabras[1]); //ID segmento
			instruccion->paramIntB = atoi(palabras[2]); //Tamaño del segmento
			//Anular el resto de parametros no usados
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "DELETE_SEGMENT") == 0)
		{
			instruccion->tipo = DELETE_SEGMENT;
			instruccion->paramIntA = atoi(palabras[1]); //ID segmento
			//Anular el resto de parametros no usados
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1; 
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "YIELD") == 0){
			instruccion->tipo = YIELD;
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1;
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}
		else if (strcmp(palabras[0], "EXIT") == 0)
		{
			instruccion->tipo = EXIT;
			//Anular el resto de parametros no usados
			instruccion->paramIntA = -1;
			instruccion->paramIntB = -1;
			instruccion->registros[0] = -1;
			instruccion->registros[1] = -1;
			instruccion->cadenaRegistro = "";
			instruccion->recurso = "";
			instruccion->nombreArchivo = "";
			free(palabras[0]);
			free(palabras[1]);
			free(palabras[2]);
		}

		//Agrego la instruccion a la lista de instrucciones
		list_add(instrucciones->listaInstrucciones, instruccion);

		//Imprimir instrucciones para ver que se hayan leido bien
		if(instruccion->tipo == EXIT)
			log_info(logger, "%s", nombresInstrucciones[instruccion->tipo]);
		/*if(instruccion->tipo == YIELD)
			log_info(logger, "%s", nombresInstrucciones[instruccion->tipo]);*/
		if(instruccion->tipo == SET)
			log_info(logger, "%s %s %s", nombresInstrucciones[instruccion->tipo], nombresRegistros[instruccion->registros[0]], instruccion->cadenaRegistro);
		if(instruccion->tipo == MOV_IN)
			log_info(logger, "%s %s %d", nombresInstrucciones[instruccion->tipo], nombresRegistros[instruccion->registros[0]], instruccion->paramIntA);
		if(instruccion->tipo == MOV_OUT)
			log_info(logger, "%s %d %s", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA, nombresRegistros[instruccion->registros[0]]);
		if(instruccion->tipo == WAIT || instruccion->tipo == SIGNAL)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->recurso);
		if(instruccion->tipo == IO)
			log_info(logger, "%s %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA);
		if(instruccion->tipo == F_OPEN)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo);
		if(instruccion->tipo == F_TRUNCATE || instruccion->tipo == F_SEEK)
			log_info(logger, "%s %s %d", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo, instruccion->paramIntA);
		if(instruccion->tipo == CREATE_SEGMENT || instruccion->tipo == F_WRITE || instruccion->tipo == F_READ)
			log_info(logger, "%s %d %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA, instruccion->paramIntB );
		if(instruccion->tipo == DELETE_SEGMENT)
			log_info(logger, "%s %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA);
		if(instruccion->tipo == F_CLOSE)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo);
		free(palabras);
	}

	fclose(archivoInstrucciones);

	log_info(logger, "Se parsearon %d instrucciones.", list_size(instrucciones->listaInstrucciones));
}

t_registro devolverRegistro(char *registro){

	if (strcmp(registro, "AX") == 0 || strcmp(registro, "AX\n") == 0)
	{
		return AX;
	}
	else if (strcmp(registro, "BX") == 0 || strcmp(registro, "BX\n") == 0)
	{
		return BX;
	}
	else if (strcmp(registro, "CX") == 0 || strcmp(registro, "CX\n") == 0)
	{
		return CX;
	}
	else if (strcmp(registro, "DX") == 0 || strcmp(registro, "DX\n") == 0)
	{
		return DX;
	}
}

/*t_instrucciones *crearListaInstrucciones(){
	t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
	instrucciones->listaInstrucciones = list_create();
	return instrucciones;
}*/

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