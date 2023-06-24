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

#define NUMBER_OF_ARGS_REQUIRED 2

int socketKernel, socketCPU, socketFilesystem;
char* IP_MEMORIA = "127.0.0.1";

t_list* lista_de_segmentos ;// aca van los segmentos generales 

t_segmento* segmento_0 ;//segemeto global va en todas las listas de segmentos
t_list * lista_de_huecos;
list_add(lista_de_segmentos,segmento_0);


//Archivo de Configuaracion
typedef struct
{
    char* puerto_escucha;//(Este es el generico )
    uint32_t tam_memoria;
    uint32_t tam_segmento_O;
    uint32_t cant_segmentos;
    uint32_t retardo_memoria;
    uint32_t retardo_compatacion;
    char* algoritmo_asignacion;
    
    //Estan puesto pero nose para que 
    /*char* PUERTO_ESCUCHA_KERNEL;
    char* PUERTO_ESCUCHA_CPU;
    char* PUERTO_ESCUCHA_FILESYSTEM;*/
} t_memoria_config;

typedef struct 
{
    //Aca tengo que ve como continuar 
}registro_EU;


typedef struct 
{   t_list* segmentos;
    int id_proces;
}t_segmentos_por_proceso;

typedef struct 
{   
    uint32_t id_segmento;
    uint32_t base;
    uint32_t tamanio;
    //int libre;
}t_segmento;



typedef struct 
{
 uint32_t base;
 u_int32_t tamanio;
}t_hueco;





t_memoria_config* leerConfiguracion();

t_memoria_config* configuracionMemoria;

void crear_hilos_memoria();
void iniciar_servidor_hacia_kernel();
void iniciar_servidor_hacia_cpu();
void iniciar_servidor_hacia_filesystem();

#endif