#include "kernel.h"

void crear_hilo_memoria(){
    // Conexión con Memoria
    int conexionConMemoria = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA);
    if (conexionConMemoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando KERNEL...");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }

    stream_send_empty_buffer(conexionConMemoria, HANDSHAKE_kernel);
    uint8_t memoriaResponse = stream_recv_header(conexionConMemoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }
    log_info(logger, "KERNEL se conectó con Memoria");

	while (1){

		
	}
}