#ifndef CONSOLA_H
#define CONSOLA_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <string.h>
#include "shared_utils.h"
#include "buffer.h"
#include "stream.h"
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

t_consola_config leerConfiguracion();
void verificacionDeConfiguracion(int argc);
FILE *abrirArchivo(char *filename);
void agregarInstruccionesDesdeArchivo(t_buffer *buffer, t_instrucciones *instrucciones, FILE* archivoInstrucciones);
t_registro devolverRegistro(char*);
char *recibirMensaje(int socket);
void *recibirStream(int socket, size_t stream_size);
void liberar_instrucciones(t_instrucciones *intrucciones);
t_instrucciones get_instrucciones(t_instrucciones* instrucciones);
void enviar_instrucciones_a_kernel(t_buffer *instructionsBuffer, t_instrucciones* instrucciones, int conexionKernel);
void consola_destroy(t_consola_config consolaConfig);
void configuracionConsola_destroy(t_consola_config consolaConfig);

#endif