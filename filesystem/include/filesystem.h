#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include <string.h>

#define CONFIG_PATH "./cfg/filesystem.cfg"
#define LOG_PATH "./cfg/filesystem.log"
#define MODULE_NAME "Filesystem"

typedef struct
{
    //datos para filesystem como cliente
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    //datos para filesystem como servidor
    char* PUERTO_ESCUCHA;
} t_filesystem_config;

void crear_hilos_filesystem();
void crear_hilo_kernel();
void crear_hilo_memoria();
t_filesystem_config* leerConfiguracion();
t_filesystem_config* configuracionFilesystem;



#endif