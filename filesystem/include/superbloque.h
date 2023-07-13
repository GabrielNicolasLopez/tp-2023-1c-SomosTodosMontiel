#ifndef SUPERBLOQUE_H_
#define SUPERBLOQUE_H_

    #include "filesystem.h"

    int levantar_volumen();
    void levantar_superbloque();

    t_superbloque_config* leerConfiguracion_superbloque();

    // Existe = 1 | NoExiste = 0
    bool existe_archivo(const char* ruta_archivo);


#endif