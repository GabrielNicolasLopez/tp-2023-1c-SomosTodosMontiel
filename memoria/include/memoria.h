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


//Variables globales de memoria
t_list* lista_de_segmentos;
t_segmento* segmento_0;
t_list* lista_de_huecos;


//Estruturas 
typedef struct
{   char* puerto_escucha;//(Este es el generico )
    uint32_t tam_memoria;
    uint32_t tam_segmento_O;
    uint32_t cant_segmentos;
    uint32_t retardo_memoria;
    uint32_t retardo_compatacion;
    char* algoritmo_asignacion;
    
} t_memoria_config;


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