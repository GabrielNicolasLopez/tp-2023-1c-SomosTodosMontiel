#include "kernel.h"

void crear_hilo_filesystem(){
	// Me conecto a filesystem
	conexion_con_fs = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM);

	if (conexion_con_fs == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON FS. FINALIZANDO KERNEL...");
		//kernel_destroy(configuracionKernel, logger);
		exit(-1);
	}

	/*stream_send_empty_buffer(conexion_con_fs, HANDSHAKE_kernel);
    t_handshake fsResponse = stream_recv_header(conexion_con_fs);

    if (fsResponse != HANDSHAKE_ok_continue)
	{
        log_error(logger, "Error al hacer handshake con módulo FS");
        //kernel_destroy(configuracionKernel, logger);
        exit(-1);
    }*/
	
	log_debug(logger, "KERNEL SE CONECTO CON FS");
	//t_buffer* buffer = buffer_create();
	//t_instruccion* p_instruccion;

	while (1){
		/*
		uint8_t fsResponse = stream_recv_header(conexion_con_fs);
		switch(fsResponse){
			case FS_OPEN_OK:
			stream_recv_buffer(conexion_con_fs, buffer);
			//Longitud cadena
			buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
			instruccion->cadena = malloc(instruccion->longitud_cadena);
			//Cadena
			buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);

			t_entradaTGAA *entradaTGAA = malloc(t_entradaTAAP);
			entradaTGAA -> nombreArchivo = instruccion->cadena;

			pthread_mutex_init(&entradaTGAA->mutex_archivo, NULL);

			break;
			case FS_OPEN_NO_OK:
			// LONGITUD_ARCHIVO
			buffer_pack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
			// CADENA_ARCHIVO
			buffer_pack(buffer, instruccion->cadena, instruccion->longitud_cadena);
			stream_send_buffer(conexion_con_fs, F_CREATE, buffer);
			break;
			case FS_CREATE_OK:
			agregarEnTAAP(t_entradaTAAP *entradaTAAP);

			stream_send_buffer(conexion_con_fs, F_CREATE, buffer);
			break;
			case :
			break;
			case :
			break;
		}
	*/	
	}
}
/*
void agregarEnTGAA(t_entradaTGAA *entradaTGAA)
{
	pthread_mutex_lock();
	list_add(tgaa, entradaTGAA);
	pthread_mutex_unlock(&pcb->mutex_TAAP);
}

*/
