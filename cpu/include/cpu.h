#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include <string.h>
#include "shared_utils.h"
#include "tests.h"

#define CONFIG_PATH "./cfg/cpu.cfg"
#define LOG_PATH "./cfg/cpu.log"
#define MODULE_NAME "CPU"

typedef struct 
{
	char* PUERTO_ESCUCHA;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
}t_cpu_config;

t_cpu_config *configuracionCPU;
t_cpu_config *leerConfiguracion();

//void crear_hilos_cpu();
void crear_hilo_kernel();
void crear_pcb();

#endif