#ifndef ESTRUCTURAS_GLOBALES_H
#define ESTRUCTURAS_GLOBALES_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h>
#include <shared_utils.h>

typedef struct
{
    uint32_t id;
    uint32_t program_counter;
    uint32_t socket;
    t_instrucciones *instrucciones;
    // Agregar lo que falta de la consigna
} t_pcb;

#endif