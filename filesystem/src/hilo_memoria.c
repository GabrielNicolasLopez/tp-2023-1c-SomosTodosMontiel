#include "filesystem.h"

// *** HILO MEMORIA ***
void* crear_hilo_memoria()
{
    // CONEXION COMO CLIENTE CON MEMORIA
    int conexion_con_memoria = crear_conexion(configuracionFS->IP_MEMORIA, configuracionFS->PUERTO_MEMORIA, logger);
    if (conexion_con_memoria == -1) {
        log_error(logger, "Error al iniciar conexion con la memoria, abortando...");
    }
    else {
        log_info(logger, "Conexion \"FS - Memoria\" exitosa");
        // HANDSHAKE
    }


    
    int* int_return = malloc(sizeof(int));
    *int_return = 0;
    return int_return;
}