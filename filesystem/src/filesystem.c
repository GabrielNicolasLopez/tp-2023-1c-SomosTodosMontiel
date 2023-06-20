#include "filesystem.h"

t_filesystem_config* configFS;
t_superbloque_config* configuracion_super_bloque;
t_FCB_config* configuracion_FCB;

int main(int argc, char ** argv){
    if (argc != 2) {
        fprintf(stderr, "Se esperaba: %s [CONFIG_PATH] - Abortando...", argv[0]);
        exit(EXIT_FAILURE);
    }
	char *CONFIG_PATH = argv[1];

	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);
    if (logger == NULL) {
        fprintf(stderr, "Error al abrir el logger, abortando...");
        exit(EXIT_FAILURE);
    }
    log_info(logger, "INICIANDO FILESYSTEM...");

    // CONFIG
    t_config *config = config_create(CONFIG_PATH);
    configFS = leerConfiguracion(config);

    iniciar_listas_y_sem();

    /*// CONEXION COMO CLIENTE CON MEMORIA
    int conexionConMemoria = crear_conexion(configFS->IP_MEMORIA, configFS->PUERTO_MEMORIA);
    if (conexionConMemoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando FS...");
        config_destroy(config);
	    free(configFS);
        exit(-1);
    }
    // Handshake
    stream_send_empty_buffer(conexionConMemoria, HANDSHAKE_filesystem);
    uint8_t memoriaResponse = stream_recv_header(conexionConMemoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        config_destroy(config);
	    free(configFS);
        exit(-1);
    }
    log_info(logger, "FS se conectó con Memoria");
    */
    // LEVANTO ARCHIVO DE SUPERBLOQUE
    t_config* config_superbloque = config_create(configFS->PATH_SUPERBLOQUE);
    configuracion_super_bloque = leerConfiguracion_superbloque(config_superbloque);
    
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
    char* p_bitmap;
    t_bitarray* bA_bitmap;
    int fd_bitmap = open(configFS->PATH_BITMAP, O_RDWR);
    
    // Guardo los atributos de ese archivo en stats_fd_bitmap
    struct stat stats_fd_bitmap;
    fstat(fd_bitmap, &stats_fd_bitmap);

    p_bitmap = mmap(NULL, stats_fd_bitmap.st_size, PROT_WRITE, MAP_SHARED, fd_bitmap, 0);
    close(fd_bitmap);
    bA_bitmap = bitarray_create_with_mode(p_bitmap, stats_fd_bitmap.st_size, LSB_FIRST);

    // recorrer el directorio de FCBs para crear las estructuras administrativas que le permita administrar los archivos

    // CREACION DE HILOS
    crear_hilos_filesystem();


    // ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
    // Liberando config
    config_destroy(config);
	free(configFS);
    // Liberando bitmap
    bitarray_destroy(bA_bitmap);
    munmap(p_bitmap, stats_fd_bitmap.st_size);

}


// CREACION DE HILOS
void crear_hilos_filesystem()
{
	pthread_t hiloMemoria, hiloKernel;
	
    pthread_create(&hiloMemoria, NULL, &crear_hilo_memoria, NULL);
    pthread_create(&hiloKernel, NULL, &crear_hilo_kernel, NULL);

    
    int* hiloMemoria_return;
    pthread_join(hiloMemoria, (void**) &hiloMemoria_return);
    if (*hiloMemoria_return) {
        log_error(logger, "Error en HILO_MEMORIA");
    }
    free(hiloMemoria_return);
    log_debug(logger, "Retorno de HILO_MEMORIA correcto");
    
    int* hiloKernel_return;
    pthread_join(hiloKernel, (void**) &hiloKernel_return);
    if (!*hiloKernel_return) {
        log_error(logger, "Error en HILO_KERNEL");
    }
    free(hiloKernel_return);
    log_debug(logger, "Retorno de HILO_KERNEL correcto");
    
    
}

// CONFIGURACION FS
t_filesystem_config *leerConfiguracion(t_config* config)
{
    t_filesystem_config* configuracion = malloc(sizeof(t_filesystem_config));

	configuracion->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracion->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    configuracion->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    configuracion->PATH_SUPERBLOQUE = config_get_string_value(config, "PATH_SUPERBLOQUE");
    configuracion->PATH_BITMAP = config_get_string_value(config, "PATH_BITMAP");
    configuracion->PATH_BLOQUES = config_get_string_value(config, "PATH_BLOQUES");
    configuracion->PATH_FCB = config_get_string_value(config, "PATH_FCB");
    configuracion->RETARDO_ACCESO_BLOQUE = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

	return configuracion;
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

size_t tamanioFCB(t_FCB_config* fcb) 
{
    return string_length(fcb->NOMBRE_ARCHIVO) + sizeof(uint32_t)*3;
}

void crear_bitmap() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BITMAP, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < configuracion_super_bloque->BLOCK_COUNT/8; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bitmap de %i bytes", configuracion_super_bloque->BLOCK_COUNT/8);

    fclose(archivo);
}

void crear_archivo_de_bloques() 
{
    // Creo el archivo
    FILE* archivo = fopen(configFS->PATH_BLOQUES, "wb+");
    
    // Lo inicializo en 0
    int a_escribir = 0;
    for (int i = 0; i < configuracion_super_bloque->BLOCK_COUNT * configuracion_super_bloque->BLOCK_SIZE; i++) {
        fputc(a_escribir, archivo);
    }
    log_debug(logger, "Crando Bloques de %i bytes", configuracion_super_bloque->BLOCK_COUNT * configuracion_super_bloque->BLOCK_SIZE);

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