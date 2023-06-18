#include "filesystem.h"

// *** HILO MEMORIA ***
void hilo_memoria(){        

    // Conexión con Memoria
    int conexionConMemoria = crear_conexion(configuracionFilesystem->IP_MEMORIA, configuracionFilesystem->PUERTO_MEMORIA);
    if (conexionConMemoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando FS...");
        filesystem_destroy(configuracionFilesystem);
        exit(-1);
    }

    stream_send_empty_buffer(conexionConMemoria, HANDSHAKE_filesystem);
    uint8_t memoriaResponse = stream_recv_header(conexionConMemoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        filesystem_destroy(configuracionFilesystem);
        exit(-1);
    }
    log_info(logger, "FS se conectó con Memoria");

    //Hacer las cosas con memoria
    //TO-DO

    while(1){}

}