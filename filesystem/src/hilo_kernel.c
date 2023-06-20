#include "hilo_kernel.h"

int socketKernel;

t_list* lista_inst;
pthread_mutex_t mutex_lista;
sem_t cant_inst;

// *** HILO KERNEL ***
void* crear_hilo_kernel()
{
    int* int_return = malloc(sizeof(int));
    *int_return = 0;

    // CREO SERVER PARA CONEXION COMO SERVIDOR CON KERNEL
    int server_fd_kernel = iniciar_servidor("127.0.0.1", configFS->PUERTO_ESCUCHA);
    log_info(logger, "Filesystem listo para recibir a Kernel");

    if (server_fd_kernel == -1) {
        log_error(logger, "Error al intentar iniciar servidor");
        *int_return = -1;
    }
	socketKernel = esperar_cliente(server_fd_kernel);
    uint8_t handshake = stream_recv_header(socketKernel);
    if (handshake == HANDSHAKE_kernel) {
        log_info(logger, "Se envia handshake ok continue a kernel");
        stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);
	}
    log_info(logger, "FS se conectó con kernel");

    // PRODUCTOR DE INSTRUCCIONES
    iniciar_listas_y_sem();
    pthread_t hilo_productor, hilo_consumidor;
    
    pthread_create(&hilo_productor, NULL, (void *)crear_hilo_productor, NULL);
    pthread_create(&hilo_consumidor, NULL, (void *)crear_hilo_consumidor, NULL);

    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);

    listas_y_sem_destroy();

    return int_return;
}

void iniciar_listas_y_sem()
{
    lista_inst = list_create();

    pthread_mutex_init(&mutex_lista, NULL);

    sem_init(&cant_inst, 0, 0);
}

void listas_y_sem_destroy()
{
    list_destroy(lista_inst);

    pthread_mutex_destroy(&mutex_lista);

    sem_destroy(&cant_inst);
}
