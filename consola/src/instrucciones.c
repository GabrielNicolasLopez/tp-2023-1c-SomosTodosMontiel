#include "instrucciones.h"

void agregarInstruccionesDesdeArchivo(FILE *instructionsFile, t_list *instrucciones)
{
	if (instructionsFile == NULL)
	{
		log_error(logger,
				  "Error: no se pudo abrir el archivo de instrucciones.");
		exit(1);
	}
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	while (fgets(buffer, 256, instructionsFile) != NULL)
	{
		char **palabra = string_split(buffer, " ");
		t_instruccion *instr = malloc(sizeof(t_instruccion));
		

		if (strcmp(palabra[0], "SET") == 0)
		{
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
}


t_registro devolverRegistro(char *registro)
{

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