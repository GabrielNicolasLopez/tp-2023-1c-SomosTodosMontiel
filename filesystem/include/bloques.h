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
    
    /*  Prametros: FCB del archivo a escribir | Puntero donde empezar a escribir | Cant. Bytes a escribir | Cadena de Bytes a escribir
        Retorno: Cantidad de bytes escritos
    */
    int escribir_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes, uint8_t* cadena_bytes);
    
    /*  Prametros: Bloque a leer | Desplazamiento del bloque donde empezar a leer | registro donde guardar lo leido | tamaño del registro
        Retorno: Cantidad de bytes leidos 
    */
    int leer_bloque(uint32_t bloque, off_t offset, void* reg, size_t tamanio);

    /*  Prametros: FCB del archivo a leer | Puntero de posición del archivo | Cant. Bytes a leer
        Retorno_OK: Cadena de bytes leidos
    */
    uint8_t* leer_bloques(t_lista_FCB_config* FCB, uint32_t puntero_archivo, uint32_t cant_bytes);

    /*  Prametros: Indice de bloque | FCB donde buscar el puntero a bloque
        Retorno_OK: Puntero a bloque
    */
    uint32_t buscar_bloque(int numero_bloque, t_lista_FCB_config* FCB);

#endif