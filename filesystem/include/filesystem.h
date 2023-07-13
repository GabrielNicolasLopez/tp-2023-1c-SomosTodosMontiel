#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
    
    #include <math.h>

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
    #include "FS_memoria.h"

    #include "estructuras.h"
    #include "variables.h"
   
    #include "configFS.h"
    #include "superbloque.h"
    #include "FCB.h"
    #include "bitmap.h"
    #include "bloques.h"
    #include "hilo_productor.h"
    #include "hilo_consumidor.h"

    //#define LOG_PATH "./cfg/filesystem.log" LOG QUE PERSISTE EN EL REPO REMOTO
    #define LOG_PATH "./cfg/filesystemPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
    #define MODULE_NAME "Filesystem"

    // PRODUCTOR CONSUMIDOR
    void iniciar_listas_y_sem();
    void listas_y_sem_destroy();

    // HILOS PARA MANEJAR LAS INSTRUCCIONES ENVIADAS POR KERNEL
    void crear_hilos_productor_consumidor();

    // ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
    void liberar_memoria();

#endif