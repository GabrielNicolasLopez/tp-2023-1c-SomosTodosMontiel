#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

    #include "filesystem.h"

    typedef struct
    {
        int BLOCK_SIZE;
        int BLOCK_COUNT;

    } t_superbloque_config;
    
    typedef struct
    {
        char* NOMBRE_ARCHIVO;
        uint32_t TAMANIO_ARCHIVO;
        uint32_t PUNTERO_DIRECTO;
        uint32_t PUNTERO_INDIRECTO;

    } t_FCB_config ;



    extern t_superbloque_config* config_SupBloque;
    extern t_FCB_config* config_FCB;

    extern struct stat stats_fd_bitmap;
    extern char* p_bitmap;
    extern t_bitarray* bitA_bitmap;



    void levantar_volumen();
    // CREACION DE ARCHIVOS EN LA CARPTA FS
    t_superbloque_config* leerConfiguracion_superbloque(t_config* config_superbloque);
    t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB);
    
    void crear_bitmap();
    void crear_archivo_de_bloques();
    void crear_archivo_de_FCB();

    void levantar_bitmap();

    // Indica si existe o no un archivo
    bool existe_archivo(const char* ruta_archivo);

#endif