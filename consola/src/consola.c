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
	FILE *archivoInstrucciones = abrirArchivo(argv[2], logger);

	t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
	instrucciones->listaInstrucciones = list_create();

	//Agrego las instrucciones del archivo a la lista
	agregarInstruccionesDesdeArchivo(instrucciones, archivoInstrucciones);

	//Creo el paquete con las instrucciones
	t_paquete *paqueteInstrucciones = crear_paquete_instrucciones(instrucciones);
	
	//Consola se conecta a kernel
	conexionKernel = crear_conexion(consola_config.ip, consola_config.puerto, logger);

	//log_info(logger, "CONSOLA-KERNEL");
	//enviar_mensaje("HOLA SOY LA CONSOLA", conexionKernel);
	
	//Enviamos el paquete
	enviar_paquete(paqueteInstrucciones, conexionKernel);
	//Borramos el paquete
	eliminar_paquete(paqueteInstrucciones);
	//Liberamos la memoria de las instrucciones
	//limpiarInformacion(instrucciones);

	log_info(logger, "INSTRUCCIONES ENVIADAS, ESPERANDO...");
	
	char *mensaje = recibirMensaje(conexionKernel);
	log_info(logger, "Kernel dice: %s", mensaje);

	//Intentar cambiar el while(1) por semáforos
	while(1){
		log_info(logger, "Consola en espera de nuevos mensajes del kernel..");
		t_paquete *paquete = recibir_paquete(conexionKernel);
		switch (paquete->codigo_operacion){
		case TERMINAR_CONSOLA:
			log_info(logger , "FINALIZANDO LA CONSOLA");
			liberar_conexion(conexionKernel);
			//Destruyo el logger
    		log_destroy(logger);
			return EXIT_SUCCESS;
		}
	}
}

char *recibirMensaje(int socket){
	size_t *tamanio_mensaje;
	char *msj;

	tamanio_mensaje = recibirStream(socket, sizeof(*tamanio_mensaje));

	if (tamanio_mensaje ){
		if ((msj = recibirStream(socket, *tamanio_mensaje))){
			free(tamanio_mensaje);
			return msj;
		}
		free(tamanio_mensaje);
	}
	return NULL;
}

void *recibirStream(int socket, size_t stream_size){
	void *stream = malloc(stream_size);

	if (recv(socket, stream, stream_size, 0) == -1)
	{
		free(stream);
		stream = NULL;
		exit(-1);
	}

	return stream;
}

int calcularSizeListaInstrucciones(t_instrucciones *instrucciones){
	int total = 0;
	for (int i = 0 ; i < list_size(instrucciones->listaInstrucciones); i++){

		t_instruccion* instruccion = list_get(instrucciones->listaInstrucciones,i);
		total += sizeof(t_tipoInstruccion);
		total += sizeof(t_registro) * 2;
		total += sizeof(uint32_t) * 2;
		total += sizeof(instruccion->recurso);
		total += sizeof(instruccion->nombreArchivo);
		total += sizeof(instruccion->cadenaRegistro);
	}
	return total;
}

t_paquete *crear_paquete_instrucciones(t_instrucciones *instrucciones){
	log_info(logger,"Empiezo a serializar las instrucciones");
	t_buffer *buffer = malloc(sizeof(t_buffer));
	buffer->size = 	sizeof(uint32_t) //Cantidad de instrucciones
					+ calcularSizeListaInstrucciones(instrucciones); // Peso de las instrucciones

	void *stream = malloc(buffer->size);

	int offset = 0; // Desplazamiento
	memcpy(stream + offset, &(instrucciones->cantidadInstrucciones), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	// Serializa las instrucciones
	int i = 0;

	while (i < list_size(instrucciones->listaInstrucciones)){
		t_instruccion* instrucccion = list_get(instrucciones->listaInstrucciones, i);
		memcpy(stream + offset,&instrucccion->tipo, sizeof(t_tipoInstruccion));
		offset += sizeof(t_tipoInstruccion);
		memcpy(stream + offset,&instrucccion->registros[0], sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(stream + offset,&instrucccion->registros[1], sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(stream + offset,&instrucccion->paramIntA, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset,&instrucccion->paramIntB, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset,&instrucccion->recurso, sizeof(instrucccion->recurso));
		offset += sizeof(instrucccion->recurso);
		memcpy(stream + offset,&instrucccion->cadenaRegistro, sizeof(instrucccion->cadenaRegistro));
		offset += sizeof(instrucccion->cadenaRegistro);
		memcpy(stream + offset,&instrucccion->nombreArchivo, sizeof(instrucccion->nombreArchivo));
		offset += sizeof(instrucccion->nombreArchivo);
		i++;
	}

	buffer->stream = stream; // Payload

	// free(informacion->instrucciones);
	// free(informacion->segmentos);

	// lleno el paquete
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW;
	paquete->buffer = buffer;
	return paquete;
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
			instruccion->cadenaRegistro = malloc(sizeof(instruccion->cadenaRegistro));
			strcpy(instruccion->cadenaRegistro, palabras[2]);
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
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
		}
		else if (strcmp(palabras[0], "F_WRITE") == 0)
		{
			instruccion->tipo = F_WRITE;
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
			/*
			char * nombreArchivo = string_new();
			string_append(&nombreArchivo, palabras[1]);
			instruccion->cadenaRegistro = nombreArchivo;
			*/
			instruccion->nombreArchivo = malloc(sizeof(instruccion->nombreArchivo));
			strcpy(instruccion->nombreArchivo, palabras[1]);
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
			/*
			char * recurso = string_new();
			string_append(&recurso, palabras[1]);
			instruccion->cadenaRegistro = recurso;
			*/
			instruccion->recurso = malloc(sizeof(instruccion->recurso));
			strcpy(instruccion->recurso, palabras[1]);
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
			/*
			char *recurso = string_new();
			string_append(&recurso, palabras[1]);
			instruccion->recurso = recurso;
			*/
			instruccion->recurso = malloc(sizeof(instruccion->recurso));
			strcpy(instruccion->recurso, palabras[1]);
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
		else if (strcmp(palabras[0], "YIELD") == 0 || strcmp(palabras[0], "YIELD\n") == 0){
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

		//printf("instruccion: %s\n", nombresInstrucciones[instruccion->tipo]);

		//Imprimir instrucciones para ver que se hayan leido bien
		if(instruccion->tipo == EXIT)
			log_info(logger, "%s", nombresInstrucciones[instruccion->tipo]);
		else if(instruccion->tipo == YIELD)
			log_info(logger, "%s", nombresInstrucciones[instruccion->tipo]);
		else if(instruccion->tipo == SET)
			log_info(logger, "%s %s %s", nombresInstrucciones[instruccion->tipo], nombresRegistros[instruccion->registros[0]], instruccion->cadenaRegistro);
		else if(instruccion->tipo == MOV_IN)
			log_info(logger, "%s %s %d", nombresInstrucciones[instruccion->tipo], nombresRegistros[instruccion->registros[0]], instruccion->paramIntA);
		else if(instruccion->tipo == MOV_OUT)
			log_info(logger, "%s %d %s", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA, nombresRegistros[instruccion->registros[0]]);
		else if(instruccion->tipo == WAIT || instruccion->tipo == SIGNAL)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->recurso);
		else if(instruccion->tipo == IO)
			log_info(logger, "%s %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA);
		else if(instruccion->tipo == F_OPEN)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo);
		else if(instruccion->tipo == F_TRUNCATE || instruccion->tipo == F_SEEK)
			log_info(logger, "%s %s %d", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo, instruccion->paramIntA);
		else if(instruccion->tipo == CREATE_SEGMENT)
			log_info(logger, "%s %d %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA, instruccion->paramIntB );
		else if(instruccion->tipo == F_WRITE || instruccion->tipo == F_READ)
			log_info(logger, "%s %s %d %d", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo, instruccion->paramIntA, instruccion->paramIntB );
		else if(instruccion->tipo == DELETE_SEGMENT)
			log_info(logger, "%s %d", nombresInstrucciones[instruccion->tipo], instruccion->paramIntA);
		else if(instruccion->tipo == F_CLOSE)
			log_info(logger, "%s %s", nombresInstrucciones[instruccion->tipo], instruccion->nombreArchivo);
		
		free(palabras);
	}

	fclose(archivoInstrucciones);

	log_info(logger, "Se parsearon %d instrucciones.", list_size(instrucciones->listaInstrucciones));
}

t_registro devolverRegistro(char *registro){

	if (strcmp(registro, "AX") == 0 || strcmp(registro, "AX\n") == 0)
		return AX;
	else if (strcmp(registro, "BX") == 0 || strcmp(registro, "BX\n") == 0)
		return BX;
	else if (strcmp(registro, "CX") == 0 || strcmp(registro, "CX\n") == 0)
		return CX;
	else if (strcmp(registro, "DX") == 0 || strcmp(registro, "DX\n") == 0)
		return DX;
	return -1;
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