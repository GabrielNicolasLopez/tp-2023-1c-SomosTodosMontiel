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
}t_configuracion_CPU;

t_configuracion_CPU *configuracion_CPU;

t_configuracion_CPU *leerConfiguracion(t_log* logger);
//void crear_hilos_cpu();
void conectar_kernel();
void crear_pcb();

#endif