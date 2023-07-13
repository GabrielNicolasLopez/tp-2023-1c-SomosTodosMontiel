#ifndef VARIABLES_H_
#define VARIABLES_H_

    #include "filesystem.h"

    // LISTAS
    extern t_list* lista_inst;
    extern t_list* l_FCBs_abiertos;

    // SEMAFOROS
    extern pthread_mutex_t mutex_lista;
    extern sem_t cant_inst;

    // SUPERBLOQUE
    extern t_config* t_config_SupBloque;
    extern t_superbloque_config* config_SupBloque;

    // BITMAP
    extern struct stat stats_fd_bitmap;
    extern char* p_bitmap;
    extern t_bitarray* bitA_bitmap;

    // BLOQUES
    extern struct stat stats_fd_bloques;
    extern char* p_bloques;

    // CONFIG
    extern t_config *config;
    extern t_filesystem_config* configFS;

    // SOCKETS
    extern int socketMemoria;
    extern int socketKernel;

#endif
