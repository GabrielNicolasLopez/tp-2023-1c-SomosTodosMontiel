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


//Archivo de Configuaracion
typedef struct
{
    char* PUERTO_ESCUCHA;//(Este es el generico )
    uint32_t tam_memoria;
    uint32_t tam_segmento_O;
    uint32_t cant_segmentos;
    uint32_t retardo_memoria;
    uint32_t retardo_compatacion;
    char* algoritmo_asignacion;
    
    //Estan puesto pero nose para que 
    char* PUERTO_ESCUCHA_KERNEL;
    char* PUERTO_ESCUCHA_CPU;
    char* PUERTO_ESCUCHA_FILESYSTEM;
} t_memoria_config;

typedef struct 
{
    //Aca tengo que ve como continuar 
}registro_EU;

typedef struct 
{
 t_list* segmentos;
    //Por ahora tiene eso por la teoria 

}tablas_segmentos;

typedef struct 
{
    int tam;
    int base;
    //Por teoria esta compuesto asi 
    int id_segmento;

}segmento;

typedef struct 
{
    // me falta pensarlo 

}huecos_libres;





t_memoria_config* leerConfiguracion();

t_memoria_config* configuracionMemoria;

void crear_hilos_memoria();
void iniciar_servidor_hacia_kernel();
void iniciar_servidor_hacia_cpu();
void iniciar_servidor_hacia_filesystem();

#endif