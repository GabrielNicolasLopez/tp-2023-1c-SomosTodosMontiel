#ifndef KERNEL_H
#define KERNEL_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include <string.h>
#include "shared_utils.h"
#include "tests.h"
#include <semaphore.h>
#include <commons/string.h>
#include <pthread.h>

#define CONFIG_PATH "./cfg/kernel.cfg"
#define LOG_PATH "./cfg/kernel.log"
#define MODULE_NAME "Kernel"

typedef struct
{
    //datos para kernel como cliente
    char* IP_MEMORIA;
    char* IP_FILESYSTEM;
    char* IP_CPU;
    char* PUERTO_MEMORIA;
    char* PUERTO_FILESYSTEM;
	char* PUERTO_CPU;
	char* ALGORITMO_PLANIFICACION;
	char* ESTIMACION_INICIAL;
	int HRRN_ALFA;
	int GRADO_MAX_MULTIPROGRAMACION;
	char** RECURSOS;
	char** INSTANCIAS_RECURSOS;
    //datos para kernel como servidor
    char* PUERTO_ESCUCHA;
} t_kernel_config;

t_kernel_config *configuracionKernel;

typedef struct
{
	char *nombre;
	int instancias_recursos;
	t_list *lista_block;
	pthread_mutex_t *mutex_lista_blocked;
    sem_t contador_bloqueo;
}t_recurso;

t_kernel_config *leerConfiguracion();
void crear_pcb();
void crear_hilo_memoria();
void crear_hilo_filesystem();
void crear_hilo_cpu();
void crear_hilo_consola();
void crear_hilos_kernel();
void cargarRecursos();
#endif