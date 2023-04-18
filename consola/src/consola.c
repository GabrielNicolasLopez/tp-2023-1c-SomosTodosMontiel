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
	//log_info(logger, "Me conecté a la ip: %s", consola_config.ip);
	//log_info(logger, "Me conecté al puerto: %s", consola_config.puerto);


	//Abro el archivo de instrucciones para sacar las instrucciones
	FILE *archivoInstrucciones = abrirArchivo(argv[2], logger);

	//Creo estructuras
	//t_instrucciones *instrucciones = crearEstructurasInstrucciones();

	//Agrego las instrucciones del archivo a la lista
	//agregarInstruccionesDesdeArchivo(instrucciones, archivoInstrucciones);

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
/*
void agregarInstruccionesDesdeArchivo(t_instrucciones *instrucciones, FILE* archivoInstrucciones){
	if (archivoInstrucciones == NULL){
		log_error(logger, "NO SE PUEDE ABRIR EL ARCHIVO DE INSTRUCCIONES, CERRANDO CONSOLA...");
		exit(1);
	}
	log_info(logger, "ARCHIVO DE INSTRUCCIONES ABIERTO, LEYENDO INSTRUCCIONES...");
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	while (fgets(buffer, 256, archivoInstrucciones) != NULL){ //Mientras pueda leer del archivo
		//Una vez que lee una linea, crea una lista con cada una de las palabras que se generan luego haberlas separado por un espacio 
		char **palabras = string_split(buffer, " "); 
		t_instruccion *instr = malloc(sizeof(t_instruccion));
		

		if (strcmp(palabra[0], "SET") == 0){
			instr->instCode = SET;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramInt = atoi(palabra[2]);
			instr->paramReg[1] = -1; // Se asigna -1 a los parametros que no se usa en la instruccion
			instr->paramIO = "";
			instr->sizeParamIO = 1;
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "ADD") == 0)
		{
			instr->instCode = ADD;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramReg[1] = devolverRegistro(palabra[2]);
			instr->paramInt = -1;
			instr->paramIO = "";
			instr->sizeParamIO = 1;
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "MOV_IN") == 0)
		{
			instr->instCode = MOV_IN;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramInt = atoi(palabra[2]);
			instr->paramReg[1] = -1;
			instr->paramIO = "";
			instr->sizeParamIO = 1;
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "MOV_OUT") == 0)
		{
			instr->instCode = MOV_OUT;
			instr->paramInt = atoi(palabra[1]);
			instr->paramReg[0] = devolverRegistro(palabra[2]);
			instr->paramReg[1] = -1;
			instr->paramIO = "";
			instr->sizeParamIO = 1;
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "I/O") == 0)
		{
			instr->instCode = IO;
			if (strcmp(palabra[1], "TECLADO") == 0 || strcmp(palabra[1], "PANTALLA") == 0)
			{	
				char * io = string_new();
				string_append(&io,palabra[1]);
				instr->paramIO = io;
				instr->sizeParamIO = strlen(io) + 1;
				instr->paramReg[0] = devolverRegistro(palabra[2]);
				instr->paramInt = -1;
				instr->paramReg[1] = -1;
				free(palabra[0]);
				free(palabra[1]);
				free(palabra[2]);
			}
			else 
			{
				char * io = string_new();
				string_append(&io,palabra[1]);
				instr->paramIO = io;
				instr->sizeParamIO = strlen(io) + 1;
				instr->paramInt = atoi(palabra[2]);
				instr->paramReg[0] = -1;
				instr->paramReg[1] = -1;
				free(palabra[0]);
				free(palabra[1]);
				free(palabra[2]);
			}
		
		}
		
		else if (strcmp(palabra[0], "EXIT") == 0)
		{
			instr->instCode = EXIT;
			instr->paramInt = -1;
			instr->paramReg[0] = -1;
			instr->paramReg[1] = -1;
			instr->paramIO = "";
			instr->sizeParamIO = 1;
			free(palabra[0]);
		}
		list_add(instrucciones, instr);
		printf("\ninstCode: %d, Num: %d, RegCPU[0]: %d,RegCPU[1] %d, dispIO: %s\n",
			instr->instCode, instr->paramInt, instr->paramReg[0], instr->paramReg[1], instr->paramIO);

		free(palabra);
	}
	fclose(instructionsFile);
	log_info(logger, "Se parsearon #Instrucciones: %d", list_size(instrucciones));


}*/

/*t_registro devolverRegistro(char *registro){

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
}*/

/*t_instrucciones *crearEstructurasInstrucciones(){
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