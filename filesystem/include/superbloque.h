#ifndef SUPERBLOQUE_H_
#define SUPERBLOQUE_H_

    #include "filesystem.h"

    typedef struct
    {
        int BLOCK_SIZE;
        int BLOCK_COUNT;

    } t_superbloque_config;
    

    // BLOQUES
    extern struct stat stats_fd_bloques;
    extern char* p_bloques;



    int levantar_volumen();
    void levantar_superbloque();

    t_superbloque_config* leerConfiguracion_superbloque();

    // Existe = 1 | NoExiste = 0
    bool existe_archivo(const char* ruta_archivo);


#endif