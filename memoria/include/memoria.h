#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include <commons/config.h>

#define CONFIG_PATH "./cfg/memoria.cfg"
//#define LOG_PATH "./cfg/memoria.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/memoriaPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Memoria"



int socketKernel, socketCPU, socketFilesystem;
char* IP_MEMORIA = "127.0.0.1";

typedef struct
{
    char* PUERTO_ESCUCHA_KERNEL;
    char* PUERTO_ESCUCHA_CPU;
    char* PUERTO_ESCUCHA_FILESYSTEM;
} t_memoria_config;

t_memoria_config* leerConfiguracion(t_log* logger);

t_memoria_config* configuracionMemoria;

void crear_hilos_memoria();
void iniciar_servidor_hacia_kernel();
void iniciar_servidor_hacia_cpu();
void iniciar_servidor_hacia_filesystem();

#endif