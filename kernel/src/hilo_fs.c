#include "kernel.h"

void crear_hilo_filesystem(){
	// Me conecto a filesystem
	int conexion_con_fs = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM);

	if (conexion_con_fs == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON FS. FINALIZANDO KERNEL...");
		//kernel_destroy(configuracionKernel, logger);
		exit(-1);
	}

	stream_send_empty_buffer(conexion_con_fs, HANDSHAKE_kernel);
    uint8_t fsResponse = stream_recv_header(conexion_con_fs);

    if (fsResponse != HANDSHAKE_ok_continue)
	{
        log_error(logger, "Error al hacer handshake con módulo FS");
        //kernel_destroy(configuracionKernel, logger);
        exit(-1);
    }
	
	log_debug(logger, "KERNEL SE CONECTO CON FS");
	
	while (1){}
}