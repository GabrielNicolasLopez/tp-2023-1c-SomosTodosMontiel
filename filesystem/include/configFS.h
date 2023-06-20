#ifndef CONFIG_FS_H_
#define CONFIG_FS_H_

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

    extern t_config *config;
    extern t_filesystem_config* configFS;

    extern int socketMemoria;
    extern int socketKernel;

    
    t_filesystem_config* leerConfiguracion(t_config* config);
    int crear_conexion_con_memoria();
    int crear_servidor_kernel();

#endif