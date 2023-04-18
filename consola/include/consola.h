#ifndef CONSOLA_H
#define CONSOLA_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"

#define CONFIG_PATH "./cfg/consola.cfg"
#define LOG_PATH "./cfg/consola.log"
#define MODULE_NAME "consola"

typedef struct
{
	char* ip;
    char* puerto;
} t_consola_config;

typedef struct
{
	//4 bytes
    char AX;
    char BX;
    char CX;
    char DX;
    //8 bytes
    char EAX;
    char EBX;
    char ECX;
    char EDX;
    //16 bytes
    char RAX;
    char RBX;
    char RCX;
    char RDX;
} t_registro;

typedef enum
{
    //3 parámetros
    F_READ, 
    F_WRITE,
    //2 parámetros
    SET,
    MOV_IN,
    MOV_OUT, 
    F_TRUNCATE, 
    F_SEEK, 
    CREATE_SEGMENT,
    //1 parámetros
    IO, 
    WAIT, 
    SIGNAL, 
    F_OPEN, 
    F_CLOSE, 
    DELETE_SEGMENT,
    //0 parámetros
    EXIT, 
    YIELD
} t_tipoInstruccion;

typedef struct
{
	t_tipoInstruccion tipoInstruccion; //F_read ó SET, etc
	uint32_t paramInt; //
    //char* paramIO; 
	uint32_t sizeParamIO;
	t_registro paramReg[2];
} t_instruccion;


t_consola_config leerConfiguracion(t_log* logger);
void verificacionDeConfiguracion(int argc, t_log* logger);
FILE *abrirArchivo(char *filename, t_log* logger);

#endif