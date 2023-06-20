#ifndef HILO_KERNEL_H_
#define HILO_KERNEL_H_

    #include "filesystem.h"

    void* crear_hilo_kernel();

    // CONEXION CLIENTE: KERNEL
    extern int socketKernel;

    // PRODUCTOR CONSUMIDOR:
    void iniciar_listas_y_sem();
    void listas_y_sem_destroy();

    extern t_list* lista_inst;
    extern pthread_mutex_t mutex_lista;
    extern sem_t cant_inst;

#endif