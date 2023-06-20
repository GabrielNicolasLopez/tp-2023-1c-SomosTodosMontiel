#include "estructuras.h"

t_superbloque_config* config_SupBloque;
t_FCB_config* config_FCB;

struct stat stats_fd_bitmap;
char* p_bitmap;
t_bitarray* bitA_bitmap;

void levantar_volumen()
{
    // LEVANTO ARCHIVO DE SUPERBLOQUE
    log_info(logger, "Levantando ARCHIVO DE SUPERBLOQUE"); 
    t_config* config_superbloque = config_create(configFS->PATH_SUPERBLOQUE);
    config_SupBloque = leerConfiguracion_superbloque(config_superbloque);
    
    // Compruebo si se encuentra existe un arvhivo de Bitmap, en caso de que no exista lo creo 
    // y en caso de que exista seguimos con la configuración de la carpeta FS.
    if(!existe_archivo(configFS->PATH_BITMAP)) {
        log_info(logger, "Bitmap inexistente, generando archivos...");
        crear_bitmap();
        crear_archivo_de_bloques();
        crear_archivo_de_FCB();
    }
    // LEVANTO BITMAP
    log_info(logger, "Levantando BITMAP");
    int fd_bitmap = open(configFS->PATH_BITMAP, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bitmap
    fstat(fd_bitmap, &stats_fd_bitmap);

    p_bitmap = mmap(NULL, stats_fd_bitmap.st_size, PROT_WRITE, MAP_SHARED, fd_bitmap, 0);
    close(fd_bitmap);
    bitA_bitmap = bitarray_create_with_mode(p_bitmap, stats_fd_bitmap.st_size, LSB_FIRST);

    // recorrer el directorio de FCBs para crear las estructuras administrativas que le permita administrar los archivos
}

// CONFIGURACION SUPERBLOQUE
t_superbloque_config* leerConfiguracion_superbloque(t_config* config_superbloque) 
{
    t_superbloque_config* configuracion = malloc(sizeof(t_superbloque_config));

	configuracion->BLOCK_SIZE = config_get_int_value(config_superbloque, "BLOCK_SIZE");
    configuracion->BLOCK_COUNT = config_get_int_value(config_superbloque, "BLOCK_COUNT");

	return configuracion;
}

// CONFIGURACION FCB
t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB)
{
    t_FCB_config* configuracion = malloc(sizeof(t_FCB_config));

	configuracion->NOMBRE_ARCHIVO = config_get_string_value(config_FCB, "NOMBRE_ARCHIVO");
    configuracion->TAMANIO_ARCHIVO = config_get_int_value(config_FCB, "TAMANIO_ARCHIVO");
    configuracion->PUNTERO_DIRECTO = config_get_int_value(config_FCB, "PUNTERO_DIRECTO");
    configuracion->PUNTERO_INDIRECTO = config_get_int_value(config_FCB, "PUNTERO_INDIRECTO");

	return configuracion;
}

void crear_bitmap() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BITMAP, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < config_SupBloque->BLOCK_COUNT/8; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bitmap de %i bytes", config_SupBloque->BLOCK_COUNT/8);

    fclose(archivo);
}

void crear_archivo_de_bloques() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BLOQUES, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < config_SupBloque->BLOCK_COUNT * config_SupBloque->BLOCK_SIZE; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bloques de %i bytes", config_SupBloque->BLOCK_COUNT * config_SupBloque->BLOCK_SIZE);

    fclose(archivo);
}

void crear_archivo_de_FCB() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_FCB, "wb+");
    log_debug(logger, "Crando directorio de FCBs de %i bytes", 0);

    fclose(archivo);
}

bool existe_archivo(const char* ruta_archivo)
{
    FILE *file = fopen(ruta_archivo, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}