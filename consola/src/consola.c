#include "consola.h"

int main(int argc, char** argv){
	int conexionKernel;
	
	//Creo logger
	logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_DEBUG);

	log_info(logger, "INICIANDO CONSOLA...");

	//Verifico la correcta inicializacion de la consola
	verificacionDeConfiguracion(argc);

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(argv[1]);

	//Leo la configuracion y la muestro
	t_consola_config consola_config = leerConfiguracion(config);

	//Consola se conecta a kernel
	conexionKernel = crear_conexion(consola_config.ip, consola_config.puerto);

	if(conexionKernel == -1){
		log_error(logger, "CONSOLA NO SE CONECTÓ A KERNEL. FINALIZANDO CONSOLA...");
		exit(1);
	}

	log_debug(logger, "CONSOLA SE CONECTÓ A KERNEL.");

    stream_send_empty_buffer(conexionKernel, HANDSHAKE_consola);
	log_debug(logger, "CONSOLA ENVIO HANDSHAKE A KERNEL.");
    uint8_t kernelResponse = stream_recv_header(conexionKernel);
	stream_recv_empty_buffer(conexionKernel);

    if (kernelResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al intentar establecer Handshake inicial con módulo Kernel");
        //consola_destroy(consola_config, logger);
        exit(1);
    }

	log_debug(logger, "CONSOLA HIZO HANDSHAKE CON KERNEL.");

	//Abro el archivo de instrucciones para sacar las instrucciones
	FILE *archivoInstrucciones = abrirArchivo(argv[2]);

	t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
	instrucciones->listaInstrucciones = list_create();

	t_buffer *instructionsBuffer = buffer_create();
	
	//Agrego las instrucciones del archivo a instructionsBuffer
	agregarInstruccionesDesdeArchivo(instructionsBuffer, instrucciones, archivoInstrucciones);
	
	//Enviamos el paquete
	enviar_instrucciones_a_kernel(instructionsBuffer, instrucciones, conexionKernel);

	buffer_destroy(instructionsBuffer);

	//Liberamos la memoria de las instrucciones
	liberar_instrucciones(instrucciones); //REVISAR


	log_info(logger, "INSTRUCCIONES ENVIADAS, ESPERANDO...");
	
	t_Kernel_Consola razon = recibir_fin_desde_kernel(conexionKernel);

	log_error(logger, "razon: %d", razon);
	switch(razon){
		case SUCCESS:
			log_error(logger,"Finalizando consola: EXIT");
		break;
		case OUT_OF_MEMORY:
			log_error(logger,"Finalizando consola: OUT_OF_MEMORY");
		break;
		case INVALID_RESOURCE:
			log_error(logger,"Finalizando consola: INVALID_RESOURCE");
		break;
		case SEG_FAULTT:
			log_error(logger,"Finalizando consola: Segfault");
		break;
		default:
			log_error(logger,"Finalizando consola: RAZON INVALIDA");
		break;
	}
	
	//Libero la conexion
	liberar_conexion(conexionKernel);
	//Destruyo el config
	//config_destroy(config);
	//Destruyo el logger
	log_destroy(logger);
	return EXIT_SUCCESS;
}

t_Kernel_Consola recibir_fin_desde_kernel(int conexionKernel){
	return stream_recv_header(conexionKernel);
}

void enviar_instrucciones_a_kernel(t_buffer *instructionsBuffer, t_instrucciones* instrucciones, int conexionKernel){

    stream_send_buffer(conexionKernel, INSTRUCCIONES, instructionsBuffer);
	log_error(logger, "Tamaño de las instrucciones enviadas a kernel %d", instructionsBuffer->size);
    //buffer_destroy(instructionsBuffer);
}	

void agregarInstruccionesDesdeArchivo(t_buffer *buffer, t_instrucciones *instrucciones, FILE* archivoInstrucciones){
	if (archivoInstrucciones == NULL){
		log_error(logger, "NO SE PUEDE ABRIR EL ARCHIVO DE INSTRUCCIONES, ABORTANDO CONSOLA...");
		exit(1);
	}
	log_info(logger, "ARCHIVO DE INSTRUCCIONES ABIERTO, LEYENDO INSTRUCCIONES...");
	const unsigned MAX_LENGTH = 256;
	char buffer_renglon[MAX_LENGTH];

	char* cadena_pack = malloc(sizeof(cadena_pack));
	char** palabras   = malloc(sizeof(buffer_renglon));


	while (fgets(buffer_renglon, 256, archivoInstrucciones) != NULL){ //Mientras pueda leer del archivo
		//Una vez que lee una linea, crea una lista con cada una de las palabras que se generan luego haberlas separado por un espacio
		palabras = string_split(buffer_renglon, " ");

		int i = 0;
		while (palabras[i]!=NULL){
			if(string_ends_with(palabras[i],"\n")){
				char *palabraSinSaltoLinea=string_replace(palabras[i], "\n", "");
				palabras[i]= palabraSinSaltoLinea;
			}
			i++;
		}

		if (strcmp(palabras[0], "SET") == 0){
			t_tipoInstruccion instruccion = SET;
			t_registro registro = devolverRegistro(palabras[1]); // Registro
			strcpy(cadena_pack, palabras[2]);
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &registro, sizeof(registro));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "MOV_IN") == 0){
			t_tipoInstruccion instruccion = MOV_IN;
			t_registro registro = devolverRegistro(palabras[1]); // Registro
			uint32_t paramIntA = atoi(palabras[2]); //Direccion logica
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &registro, sizeof(registro));
			buffer_pack(buffer, &paramIntA, sizeof(paramIntA));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "MOV_OUT") == 0){
			t_tipoInstruccion instruccion = MOV_OUT;
			t_registro registro = devolverRegistro(palabras[2]); // Registro
			uint32_t paramIntA = atoi(palabras[1]); //Direccion logica
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &paramIntA, sizeof(paramIntA));
			buffer_pack(buffer, &registro, sizeof(registro));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "I/O") == 0){
			t_tipoInstruccion instruccion = IO;
			uint32_t paramIntA = atoi(palabras[1]); //Tiempo
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &paramIntA, sizeof(paramIntA));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_OPEN") == 0){
			t_tipoInstruccion instruccion = F_OPEN;
			strcpy(cadena_pack, palabras[1]);
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_CLOSE") == 0){
			t_tipoInstruccion instruccion = F_CLOSE;
			strcpy(cadena_pack, palabras[1]);
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_SEEK") == 0){
			t_tipoInstruccion instruccion = F_SEEK;
			strcpy(cadena_pack, palabras[1]);
			uint32_t paramIntA = atoi(palabras[2]); //Posicion del puntero
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);
			buffer_pack(buffer, &paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_READ") == 0){
			t_tipoInstruccion instruccion = F_READ;
			strcpy(cadena_pack, palabras[1]);
			uint32_t paramIntA = atoi(palabras[2]); //Direccion logica
			uint32_t paramIntB = atoi(palabras[3]); //Cantidad de bytes
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);	
			buffer_pack(buffer, &paramIntA, sizeof(uint32_t));
			buffer_pack(buffer, &paramIntB, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_WRITE") == 0){
			t_tipoInstruccion instruccion = F_WRITE;
			strcpy(cadena_pack, palabras[1]);
			uint32_t paramIntA = atoi(palabras[2]); //Direccion logica
			uint32_t paramIntB = atoi(palabras[3]); //Cantidad de bytes
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);	
			buffer_pack(buffer, &paramIntA, sizeof(uint32_t));
			buffer_pack(buffer, &paramIntB, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "F_TRUNCATE") == 0){
			t_tipoInstruccion instruccion = F_TRUNCATE;
			strcpy(cadena_pack, palabras[1]);
			uint32_t paramIntA = atoi(palabras[2]); //Tamaño del archivo
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);	
			buffer_pack(buffer, &paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "WAIT") == 0){
			t_tipoInstruccion instruccion = WAIT;
			strcpy(cadena_pack, palabras[1]);
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			//log_error(logger, "long: %d", longitud_cadena_pack);
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);	
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "SIGNAL") == 0){
			t_tipoInstruccion instruccion = SIGNAL;
			strcpy(cadena_pack, palabras[1]);
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			uint32_t longitud_cadena_pack = string_length(cadena_pack)+1;
			buffer_pack(buffer, &longitud_cadena_pack, sizeof(uint32_t));
			buffer_pack(buffer, cadena_pack, longitud_cadena_pack);		
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);
				
		}
		else if (strcmp(palabras[0], "CREATE_SEGMENT") == 0){
			t_tipoInstruccion instruccion = CREATE_SEGMENT;
			uint32_t paramIntA = atoi(palabras[1]); //ID segmento
			uint32_t paramIntB = atoi(palabras[2]); //Tamaño del segmento
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &paramIntA, sizeof(uint32_t));
			buffer_pack(buffer, &paramIntB, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "DELETE_SEGMENT") == 0){
			t_tipoInstruccion instruccion = DELETE_SEGMENT;
			uint32_t paramIntA = atoi(palabras[1]); //ID segmento
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			buffer_pack(buffer, &paramIntA, sizeof(paramIntA));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "YIELD") == 0){
			t_tipoInstruccion instruccion = YIELD;
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
		else if (strcmp(palabras[0], "EXIT") == 0){
			t_tipoInstruccion instruccion = EXIT;
			buffer_pack(buffer, &instruccion, sizeof(instruccion));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", buffer->size);

		}
	}

	fclose(archivoInstrucciones);
	//free(cadena_pack);
	//free(cadena_pack);
	/*free(palabras[0]);
	free(palabras[1]);
	free(palabras[2]);*/
	free(palabras);
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
	else if (strcmp(registro, "EAX") == 0 || strcmp(registro, "EAX\n") == 0)
		return EAX;
	else if (strcmp(registro, "EBX") == 0 || strcmp(registro, "EBX\n") == 0)
		return EBX;
	else if (strcmp(registro, "ECX") == 0 || strcmp(registro, "ECX\n") == 0)
		return ECX;
	else if (strcmp(registro, "EDX") == 0 || strcmp(registro, "EDX\n") == 0)
		return EDX;
	else if (strcmp(registro, "RAX") == 0 || strcmp(registro, "RAX\n") == 0)
		return RAX;
	else if (strcmp(registro, "RBX") == 0 || strcmp(registro, "RBX\n") == 0)
		return RBX;
	else if (strcmp(registro, "RCX") == 0 || strcmp(registro, "RCX\n") == 0)
		return RCX;
	else if (strcmp(registro, "RDX") == 0 || strcmp(registro, "RDX\n") == 0)
		return RDX;	
	return -1;
}

FILE *abrirArchivo(char *nombreArchivo){
	if (!nombreArchivo){ //Probar
		log_error(logger, "NOMBRE DE ARCHIVO ERRONEO");
		exit(1);
	}
	return fopen(nombreArchivo, "r");
}

void verificacionDeConfiguracion(int argc){
	log_info(logger, "Cantidad de parametros: %d", argc);
	if (argc != 3){ //.c, config, pseudocodigo
		log_error(logger, "CANTIDAD DE PARAMETROS INCORRECTA");
		exit(1);
	}
	else
		log_info(logger, "CONSOLA INICIALIZADA CORRECTAMENTE");
}

t_consola_config leerConfiguracion(t_config* config){

	//Creo el archivo config
	t_consola_config configuracionConsola;

	//Leo los datos del config
	configuracionConsola.ip     = config_get_string_value(config, "IP_KERNEL");
	configuracionConsola.puerto = config_get_string_value(config, "PUERTO_KERNEL");

	return configuracionConsola;
}

void liberar_instrucciones(t_instrucciones *instrucciones)
{
	for(int i=0; i<list_size(instrucciones->listaInstrucciones);i++){
		list_remove_and_destroy_element(instrucciones->listaInstrucciones, i, free);
	}
	list_destroy(instrucciones->listaInstrucciones);
	free(instrucciones);
}
