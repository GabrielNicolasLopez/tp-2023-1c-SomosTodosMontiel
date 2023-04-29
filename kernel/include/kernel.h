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
//#define LOG_PATH "./cfg/kernel.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/kernelPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
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

typedef struct
{
	char ax[4];
	char bx[4];
	char cx[4];
	char dx[4];
}t_registro;

typedef struct
{
	char eax[8];
	char ebx[8];
	char ecx[8];
	char edx[8];
}t_registroE;

typedef struct
{
	char rax[16];
	char rbx[16];
	char rcx[16];
	char rdx[16];
}t_registroR;

typedef struct
{
	t_registro *registro;
	t_registroE *registroE;
	t_registroR *registroR;
}t_registrosCPU;

typedef struct
{
	uint32_t pid;
	uint32_t program_counter;
	t_instrucciones *instrucciones;
	t_list tablaDeSegmentos;
	t_registrosCPU *registrosCPU;
}t_contextoEjecucion;



typedef struct
{
    uint32_t pid;
    uint32_t program_counter;
    uint32_t socket;
	t_contextoEjecucion *contextoEjecucion;
	t_recurso *Recurso;
} t_pcb;

t_kernel_config *leerConfiguracion();
void crear_pcb();
void crear_hilo_memoria();
void crear_hilo_filesystem();
void crear_hilo_cpu();
void crear_hilo_consola();
void crear_hilos_kernel();
void cargarRecursos();
int enviarMensaje(int socket, char *msj);
void *serializarMensaje(char *msj, size_t *size_stream);
int enviarStream(int socket, void *stream, size_t stream_size);
#endif
