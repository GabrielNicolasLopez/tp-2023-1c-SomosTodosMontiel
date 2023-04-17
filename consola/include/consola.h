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

t_consola_config leerConfiguracion(t_log* logger);
void verificacionDeConfiguracion(int argc, t_log* logger);
FILE *abrirArchivo(char *filename, t_log* logger);

#endif