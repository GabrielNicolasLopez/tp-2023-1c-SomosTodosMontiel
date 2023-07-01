#ifndef CONFIG_FS_H_
#define CONFIG_FS_H_

    #include "filesystem.h"

    extern t_config *config;
    extern t_filesystem_config* configFS;

    extern int socketMemoria;
    extern int socketKernel;

    t_filesystem_config* leerConfiguracion(t_config* config);
    int crear_conexion_con_memoria();
    int crear_servidor_kernel();

#endif