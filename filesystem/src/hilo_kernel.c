#include "filesystem.h"

// *** HILO KERNEL ***
void* crear_hilo_kernel()
{
    int* int_return = malloc(sizeof(int));
    *int_return = 0;

    // CREO SERVER PARA CONEXION COMO SERVIDOR CON KERNEL
    int socketEscucha = iniciar_servidor("127.0.0.1", configFS->PUERTO_ESCUCHA);
    log_info(logger, "Filesystem listo para recibir a Kernel");

    if (socketEscucha == -1) {
        log_error(logger, "Error al intentar iniciar servidor");
        *int_return = -1;
    }
	int socketCliente = esperar_cliente(socketEscucha);
    uint8_t handshake = stream_recv_header(socketCliente);
    if (handshake == HANDSHAKE_kernel) {
        log_info(logger, "Se envia handshake ok continue a kernel");
        stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);
	}
    log_info(logger, "FS se conectó con kernel");
    
    //Hacer las cosas con kernel
    //TO-DO

    while(1){}


    return int_return;
}

