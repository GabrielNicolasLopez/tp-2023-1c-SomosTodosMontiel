#ifndef BLOQUES_H_
#define BLOQUES_H_

    #include "filesystem.h"
    
    extern struct stat stats_fd_bloques;
    extern char* p_bloques;

    void crear_archivo_de_bloques();
    void levantar_bloques();

    void escribir_bloque(uint32_t bloque, off_t offset, void* stream, size_t tamanio);

#endif