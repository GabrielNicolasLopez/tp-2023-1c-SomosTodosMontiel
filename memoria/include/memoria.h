#ifndef MEMORIA_H
#define MEMORIA_H

#include "shared_utils.h"
#include "FS_kernel.h"
#include "buffer.h"
#include "stream.h"

#define CONFIG_PATH "./cfg/memoria.cfg"
//#define LOG_PATH "./cfg/memoria.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/memoriaPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Memoria"


#define NUMBER_OF_ARGS_REQUIRED 2

//Variables globales de memoria
extern int conexion_con_kernel;
extern int conexion_con_FileSystem;
extern int conexion_con_cpu;
extern int conexion_con_memoria;

extern pthread_t hiloFilesystem, hiloKernel, hiloCPU;

extern void* espacioUsuario;

//Listas
extern t_list* listaSegmentos;
extern t_list* listaHuecos;

extern t_segmento* segmento_0;
extern t_hueco* hueco_0;


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

typedef enum
{
    BEST,
    FIRST,
    WORST

}t_tipo_algoritmo;

t_memoria_config* leerConfiguracion();

extern t_memoria_config* configuracionMemoria;

void crear_hilos_memoria();
void recibir_conexiones(int socketEscucha);

#endif