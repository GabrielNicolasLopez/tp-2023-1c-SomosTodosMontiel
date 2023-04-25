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
#define LOG_PATH "./cfg/consola.log"
#define MODULE_NAME "consola"

typedef struct
{
    char *ip;
    char *puerto;
} t_consola_config;

const char *nombresInstrucciones[] = {"F_READ", "F_WRITE", "SET", "MOV_IN", "MOV_OUT", "F_TRUNCATE", "F_SEEK", "CREATE_SEGMENT",
                                      "IO", "WAIT", "SIGNAL", "F_OPEN", "F_CLOSE", "DELETE_SEGMENT", "YIELD", "EXIT"};
const char *nombresRegistros[] = {"AX", "BX", "CX", "DX"};

t_consola_config leerConfiguracion(t_log *logger);
void verificacionDeConfiguracion(int argc, t_log *logger);
FILE *abrirArchivo(char *filename, t_log *logger);
void agregarInstruccionesDesdeArchivo(t_instrucciones *, FILE *);
t_registro devolverRegistro(char *);
t_paquete *crear_paquete_instrucciones(t_instrucciones *instrucciones);
char *recibirMensaje(int socket);
void *recibirStream(int socket, size_t stream_size);
void quitarSaltoDeLinea(char **palabras, int lugar);
#endif