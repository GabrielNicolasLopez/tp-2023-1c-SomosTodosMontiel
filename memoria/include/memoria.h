#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include <commons/config.h>


//Variables globales con los sockets de cada módulo
extern int conexion_con_kernel;
extern int conexion_con_memoria;
extern int conexion_con_cpu;

#define CONFIG_PATH "./cfg/memoria.cfg"
//#define LOG_PATH "./cfg/memoria.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/memoriaPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Memoria"

#define NUMBER_OF_ARGS_REQUIRED 2

extern t_list* lista_de_segmentos;// aca van los segmentos generales 

extern t_segmento* segmento_0;//segemeto global va en todas las listas de segmentos

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
    // me falta pensarlo 

}huecos_libres;

t_memoria_config* leerConfiguracion();

extern t_memoria_config* configuracionMemoria;

void recibir_conexion(int server_fd);
void memoria_destroy(t_memoria_config* configuracionMemoria, t_log* logger);
void memoria_config_destroy(t_memoria_config* configuracionMemoria);

#endif