#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

    #include "filesystem.h"
    
    // CONFIG
    typedef struct
    {
        //datos para filesystem como cliente
        char* IP_MEMORIA;
        char* PUERTO_MEMORIA;
        //datos para filesystem como servidor
        char* PUERTO_ESCUCHA;
        //resto de datos
        char* PATH_SUPERBLOQUE;
        char* PATH_BITMAP;
        char* PATH_BLOQUES;
        char* PATH_FCB;
        int RETARDO_ACCESO_BLOQUE;

    } t_filesystem_config;

    // FCB
    typedef struct
    {
        char* NOMBRE_ARCHIVO;
        uint32_t TAMANIO_ARCHIVO;
        uint32_t PUNTERO_DIRECTO;
        uint32_t PUNTERO_INDIRECTO;

    } t_FCB_config ;

    typedef struct
    {
        char* nombre_archivo;
        t_config* config;
        t_FCB_config* FCB_config;
        
    } t_lista_FCB_config;

    //SUPERBLOQUE
    typedef struct
    {
        int BLOCK_SIZE;
        int BLOCK_COUNT;

    } t_superbloque_config;



#endif