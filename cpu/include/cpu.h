#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <string.h>
#include "shared_utils.h"
#include "tests.h"
#endif





typedef struct t_configuracion_CPU
	{
	//int RETARDO_INSTRUCCION;
	//string IP_MEMORIA;
	//int  PUERTO_MEMORIA;
	int PUERTO_ESCUCHA;
	//int TAM_MAX_SEGMENTO

	}t_configuracion_CPU;


t_configuracion_CPU leerConfiguracion(t_log* logger);
