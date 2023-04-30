#ifndef CONSOLA_H
#define CONSOLA_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <string.h>
#include "shared_utils.h"
#include "tests.h"

#define CONFIG_PATH "./cfg/consola.cfg"
//#define LOG_PATH "./cfg/consola.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/consolaPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "consola"

typedef struct
{
    char *ip;
    char *puerto;
} t_consola_config;


const char *nombresRegistros[] = {"AX", "BX", "CX", "DX"};


t_consola_config leerConfiguracion(t_log *logger);
void verificacionDeConfiguracion(int argc, t_log *logger);
FILE *abrirArchivo(char *filename, t_log *logger);
void agregarInstruccionesDesdeArchivo(t_instrucciones *, FILE *);
t_registro devolverRegistro(char *);
t_paquete *crear_paquete_instrucciones(t_instrucciones *instrucciones);
char *recibirMensaje(int socket);
void *recibirStream(int socket, size_t stream_size);
#endif