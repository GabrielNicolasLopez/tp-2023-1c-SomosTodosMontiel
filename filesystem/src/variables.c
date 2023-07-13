#include "variables.h"

// LISTAS
t_list* lista_inst;
t_list* l_FCBs_abiertos;

// SEMAFOROS
pthread_mutex_t mutex_lista;
sem_t cant_inst;

// SUPERBLOQUE
t_config* t_config_SupBloque;
t_superbloque_config* config_SupBloque;

// BITMAP
struct stat stats_fd_bitmap;
char* p_bitmap;
t_bitarray* bitA_bitmap;

// BLOQUES
struct stat stats_fd_bloques;
char* p_bloques;

// CONFIG
t_config *config;
t_filesystem_config* configFS;

// SOCKETS
int socketMemoria;
int socketKernel;

