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

typedef enum
{
	//4 bytes
    AX,
    BX,
    CX,
    DX,
    //8 bytes
    EAX,
    EBX,
    ECX,
    EDX,
    //16 bytes
    RAX,
    RBX,
    RCX,
    RDX
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
	t_tipoInstruccion tipo; 
	uint32_t paramIntA, paramIntB; // En caso de que se deban guardar dos int
    char* recurso; //Disco, etc
	uint32_t longitudRecurso; //Longitud de la palabra 
	t_registro registros[2]; //Puede que con 1 solo es suficiente
    char* cadena[16];
} t_instruccion;
// __attribute__((packed)) t_instruccion; esto tiene que ir?


t_consola_config leerConfiguracion(t_log* logger);
void verificacionDeConfiguracion(int argc, t_log* logger);
FILE *abrirArchivo(char *filename, t_log* logger);

#endif