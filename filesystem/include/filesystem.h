#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include <string.h>
#include "stream.h"

#define CONFIG_PATH "./cfg/filesystem.cfg"
//#define LOG_PATH "./cfg/filesystem.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/filesystemPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Filesystem"

typedef struct
{
    //datos para filesystem como cliente
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    //datos para filesystem como servidor
    char* PUERTO_ESCUCHA;
    char* PATH_SUPERBLOQUE;
    char* PATH_BITMAP;
    char* PATH_BLOQUES;
    char* PATH_FCB;
    char* RETARDO_ACCESO_BLOQUE;
} t_filesystem_config;

void crear_hilos_filesystem();
void filesystem_destroy(t_filesystem_config* filesystemConfig);
void filesystem_config_destroy(t_filesystem_config* filesystemConfig);
t_filesystem_config* leerConfiguracion();

void hilo_kernel();
void hilo_memoria();

extern t_filesystem_config* configuracionFilesystem;

#endif