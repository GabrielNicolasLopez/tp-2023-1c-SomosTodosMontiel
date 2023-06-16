#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
    
    #include "shared_utils.h"
    #include "hilo_kernel.h"
    #include "hilo_memoria.h"
    #include "tests.h"

    //#define LOG_PATH "./cfg/filesystem.log" LOG QUE PERSISTE EN EL REPO REMOTO
    #define LOG_PATH "./cfg/filesystemPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
    #define MODULE_NAME "Filesystem"

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
    
    extern t_filesystem_config* configuracionFS;
    t_filesystem_config* leerConfiguracion(t_config* config);

    // HILOS
    void crear_hilos_filesystem();

#endif