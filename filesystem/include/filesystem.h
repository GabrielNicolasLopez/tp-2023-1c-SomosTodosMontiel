#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
    
    #include <fcntl.h>

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>

    #include <sys/mman.h>

    #include <commons/bitarray.h>
    
    #include "shared_utils.h"
    #include "stream.h"
    #include "FS_kernel.h"
   
    #include "hilo_memoria.h"
    #include "hilo_kernel.h"
    #include "hilo_productor.h"
    #include "hilo_consumidor.h"
    #include "tests.h"

    //#define LOG_PATH "./cfg/filesystem.log" LOG QUE PERSISTE EN EL REPO REMOTO
    #define LOG_PATH "./cfg/filesystemPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
    #define MODULE_NAME "Filesystem"

    // CONFIG
    typedef struct
    {
        //datos para filesystem como cliente
        char* IP_MEMORIA;
        char* PUERTO_MEMORIA;
        //datos para filesystem como servidor
        char* PUERTO_ESCUCHA;
        //resto de datos
        char* PATH_SUPERBLOQUE;
        char* PATH_BITMAP;
        char* PATH_BLOQUES;
        char* PATH_FCB;
        int RETARDO_ACCESO_BLOQUE;

    } t_filesystem_config;
    
    extern t_filesystem_config* configFS;
    t_filesystem_config* leerConfiguracion(t_config* config);

    // CONFIG DE SUPERBLOQUE
    typedef struct
    {
        int BLOCK_SIZE;
        int BLOCK_COUNT;

    } t_superbloque_config;

    extern t_superbloque_config* configuracion_super_bloque;
    t_superbloque_config* leerConfiguracion_superbloque(t_config* config_superbloque);
    
    // CONFIG DE FCB
    typedef struct
    {
        char* NOMBRE_ARCHIVO;
        uint32_t TAMANIO_ARCHIVO;
        uint32_t PUNTERO_DIRECTO;
        uint32_t PUNTERO_INDIRECTO;

    } t_FCB_config ;

    extern t_FCB_config* configuracion_FCB;
    t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB);

    // CREACION DE ARCHIVOS EN LA CARPTA FS
    void crear_bitmap();
    void crear_archivo_de_bloques();
    void crear_archivo_de_FCB();

    // BITMAP
    void levantar_bitmap();

    size_t tamanioFCB(t_FCB_config* fcb);

    // Indica si existe o no un archivo
    bool existe_archivo(const char* ruta_archivo);

    // HILOS
    void crear_hilos_filesystem();

#endif