#include "estructuras.h"

// SUPERBLOQUE
t_config* t_config_SupBloque;
t_superbloque_config* config_SupBloque;

// BITMAP
struct stat stats_fd_bitmap;
char* p_bitmap;
t_bitarray* bitA_bitmap;

// BLOQUES
struct stat stats_fd_bloques;
char* p_bloques;
t_bitarray* bitA_bloques;


int levantar_volumen()
{
    levantar_superbloque();

    if(!existe_archivo(configFS->PATH_BITMAP)) {
        log_info(logger, "Bitmap inexistente, generando archivos...");
        crear_bitmap();
        crear_archivo_de_bloques();
        if (crear_carpeta_de_FCBs() == -1) {
            return -1;
        }

    }

    levantar_bitmap();
    levantar_bloques();

    crear_FCB("Hola.txt");
    bitarray_set_bit(bitA_bitmap, 5);
    
    return 0;
}

void levantar_superbloque()
{
    log_info(logger, "Levantando ARCHIVO DE SUPERBLOQUE"); 
    t_config_SupBloque = config_create(configFS->PATH_SUPERBLOQUE);
    config_SupBloque = leerConfiguracion_superbloque();
}

void levantar_bitmap()
{
    log_info(logger, "Levantando BITMAP");
    int fd_bitmap = open(configFS->PATH_BITMAP, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bitmap
    fstat(fd_bitmap, &stats_fd_bitmap);

    p_bitmap = mmap(NULL, stats_fd_bitmap.st_size, PROT_WRITE, MAP_SHARED, fd_bitmap, 0);
    close(fd_bitmap);
    bitA_bitmap = bitarray_create_with_mode(p_bitmap, stats_fd_bitmap.st_size, LSB_FIRST);
}

void levantar_bloques()
{
    log_info(logger, "Levantando BLOQUES");
    int fd_bloques = open(configFS->PATH_BLOQUES, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bloques
    fstat(fd_bloques, &stats_fd_bloques);

    p_bloques = mmap(NULL, stats_fd_bloques.st_size, PROT_WRITE, MAP_SHARED, fd_bloques, 0);
    close(fd_bloques);
    bitA_bloques = bitarray_create_with_mode(p_bloques, stats_fd_bloques.st_size, LSB_FIRST);
}

// CONFIGURACION SUPERBLOQUE
t_superbloque_config* leerConfiguracion_superbloque() 
{
    t_superbloque_config* configuracion = malloc(sizeof(t_superbloque_config));

	configuracion->BLOCK_SIZE = config_get_int_value(t_config_SupBloque, "BLOCK_SIZE");
    configuracion->BLOCK_COUNT = config_get_int_value(t_config_SupBloque, "BLOCK_COUNT");

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

bool existe_archivo(const char* ruta_archivo)
{
    FILE *file = fopen(ruta_archivo, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
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

int crear_carpeta_de_FCBs() 
{
    mode_t mode = 
        // OWNWER: Lectura, escritura y ejecucion
        S_IRWXU |
        // GROUP: Lectura, escritura y ejecucion
        S_IRWXG |
        // OTHERS: Lectura y ejecucion
        S_IROTH | S_IXOTH
        ;

    int error_mkdir = mkdir(configFS->PATH_FCB, mode);
    if (error_mkdir == -1) {
        return -1; 
    }
    return 0;
}

t_config* crear_FCB(char* archivo)
{
    log_info(logger, "Creando FCB: %s", archivo); 
    
    int fd;
    mode_t mode = 
        // OWNWER: Lectura, escritura y ejecucion
        S_IRUSR | S_IWUSR |
        // GROUP: Lectura, escritura y ejecucion
        S_IRGRP | S_IWGRP |
        // OTHERS: Lectura y ejecucion
        S_IROTH
        ;
    
    char *pathname = string_new();
	
    string_append(&pathname, configFS->PATH_FCB);
	string_append(&pathname, "/");
    string_append(&pathname, archivo);

    // CREO EL FCB: "archivo"
    fd = open(pathname, O_RDWR|O_CREAT|O_TRUNC, mode);
    close(fd);

    // Genero las keys del FCB
    t_config* config_FCB  = config_create(pathname);
    config_set_value(config_FCB, "NOMBRE_ARCHIVO", archivo);
    config_set_value(config_FCB, "TAMANIO_ARCHIVO", "0");
    config_set_value(config_FCB, "PUNTERO_DIRECTO", "-1");
    config_set_value(config_FCB, "PUNTERO_INDIRECTO", "-1");
    config_save(config_FCB);

    return config_FCB;
}

t_config* buscar_FCB(char* archivo) 
{
    char *pathname = string_new();
	
    string_append(&pathname, configFS->PATH_FCB);
	string_append(&pathname, "/");
    string_append(&pathname, archivo);

    return config_create(pathname);
}

t_FCB_config* levantar_FCB(t_config* t_config_FCB)
{
    log_info(logger, "Levantando FCB"); 
    
    t_FCB_config* config_FCB = malloc(sizeof(t_FCB_config));

    config_FCB = leerConfiguracion_FCB(t_config_FCB);

    return config_FCB;
}

bool condicion_FCB_find(t_lista_FCB_config* FCB)
{
    FCB->nombre_archivo
}

t_lista_FCB_config* FCB_list_get(char* archivo)
{
    
    list_find(l_FCBs_abiertos, archivo_corresponde_con_FCB);
}


