#ifndef HILO_PRODUCTOR_H_
#define HILO_PRODUCTOR_H_

    #include "filesystem.h"

    void crear_hilo_productor();

    t_instruccion_FS* recibir_instruccion(int socket);


#endif