#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include <commons/config.h>

#define CONFIG_PATH "./cfg/memoria.cfg"
#define LOG_PATH "./cfg/memoria.log"
#define MODULE_NAME "Memoria"

//#define IP "127.0.0.1"

typedef struct
{
    char* PUERTO_ESCUCHA;
} t_memoria_config;

t_memoria_config leerConfiguracion(t_log* logger);




#endif