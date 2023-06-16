#include "filesystem.h"

// *** HILO KERNEL ***
void* crear_hilo_kernel()
{
    // CREO SERVER PARA CONEXION COMO SERVIDOR CON KERNEL
    int server_fd = iniciar_servidor("127.0.0.1", configuracionFS->PUERTO_ESCUCHA, logger);
    if (server_fd == -1) {
        log_error(logger, "Error al iniciar servidor, abortando...");
    }
    else {
        log_info(logger, "Servidor del Filesystem listo para recibir al Kernel");
        int cliente_fd = esperar_cliente(server_fd, logger);
        //HANDSHAKE
    }

    
    //LOGICA HILO KERNEL
    while(1){
        
    }

    int* int_return = malloc(sizeof(int));
    *int_return = 0;
    return int_return;
}

