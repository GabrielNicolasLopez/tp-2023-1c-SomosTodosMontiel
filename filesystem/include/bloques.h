#ifndef BLOQUES_H_
#define BLOQUES_H_

    #include "filesystem.h"
    
    extern struct stat stats_fd_bloques;
    extern char* p_bloques;

    void crear_archivo_de_bloques();
    void levantar_bloques();

    /*  Prametros: Bloque a escribir | Desplazamiento del bloque donde empezar a escribir | registro a escribir | tamaño del registro 
        Retorno: Cantidad de bytes escritos
    */
    int escribir_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio);
    
    /*  Prametros: Bloque a leer | Desplazamiento del bloque donde empezar a leer | registro donde guardar lo leido | tamaño del registro
        Retorno: Cantidad de bytes leidos 
    */
    int leer_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio);

#endif