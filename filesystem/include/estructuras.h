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

    typedef struct
    {
        char* nombre_archivo;
        t_config* config;
        t_FCB_config* FCB_config;
    } t_lista_FCB_config;



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
    extern t_bitarray* bitA_bloques;



    int levantar_volumen();
    void levantar_superbloque();
    
    void crear_bitmap();
    void crear_archivo_de_bloques();
    int crear_carpeta_de_FCBs();
    t_config* crear_FCB(char* archivo);
    t_config* buscar_FCB(char* archivo);


    void levantar_bitmap();
    void levantar_bloques();
    t_FCB_config* levantar_FCB(t_config* t_config_FCB);

    t_superbloque_config* leerConfiguracion_superbloque();
    t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB);

    // Existe = 1 | NoExiste = 0
    bool existe_archivo(const char* ruta_archivo);

#endif