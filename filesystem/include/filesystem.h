#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
    
    #include <fcntl.h>

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>

    #include <sys/mman.h>

    #include <commons/bitarray.h>
    
    #include "shared_utils.h"
    #include "stream.h"
    #include "FS_kernel.h"
   
    #include "configFS.h"
    #include "estructuras.h"
    #include "hilo_productor.h"
    #include "hilo_consumidor.h"

    #include "tests.h"

    //#define LOG_PATH "./cfg/filesystem.log" LOG QUE PERSISTE EN EL REPO REMOTO
    #define LOG_PATH "./cfg/filesystemPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
    #define MODULE_NAME "Filesystem"

    // VARIABLES GLOBALES
    extern t_list* lista_inst;
    extern t_list* l_FCBs_abiertos;

    extern pthread_mutex_t mutex_lista;
    extern sem_t cant_inst;

    // PRODUCTOR CONSUMIDOR
    void iniciar_listas_y_sem();
    void listas_y_sem_destroy();

    // HILOS PARA MANEJAR LAS INSTRUCCIONES ENVIADAS POR KERNEL
    void crear_hilos_productor_consumidor();

    // ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
    void liberar_memoria();

#endif