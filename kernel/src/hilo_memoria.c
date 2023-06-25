#include "kernel.h"

void crear_hilo_memoria(){
    // Conexión con Memoria
    /*conexion_con_memoria = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA);
    if (conexion_con_memoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando KERNEL...");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }

    stream_send_empty_buffer(conexion_con_memoria, HANDSHAKE_kernel);
    t_handshake memoriaResponse = stream_recv_header(conexion_con_memoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }
    log_info(logger, "KERNEL se conectó con Memoria");

	while (1){

		
	}*/
}