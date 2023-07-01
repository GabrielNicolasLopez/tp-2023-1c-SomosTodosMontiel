#include "filesystem.h"

t_list* lista_inst;
t_list* l_FCBs_abiertos;

pthread_mutex_t mutex_lista;
sem_t cant_inst;

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
    config = config_create(CONFIG_PATH);
    configFS = leerConfiguracion(config);
    
    // CONEXION COMO CLIENTE CON MEMORIA
    int error_memoria = crear_conexion_con_memoria();
    if (error_memoria == -1) {
        config_destroy(config);
	    free(configFS);
        exit(-1);
    }
    
    // LEVANTO ARCHIVOS DEL VOLUMEN (CARPETA FS)
    if (levantar_volumen() == -1) {
        config_destroy(config);
	    free(configFS);
        exit(-1);
    }

    // CREO SERVER PARA CONEXION COMO SERVIDOR CON KERNEL
    if (crear_servidor_kernel() == -1) {
        log_error(logger, "Error al crear servidor con kernel");
        config_destroy(config);
	    free(configFS);
        exit(-1);
    }

    // HILOS PARA MANEJAR LAS INSTRUCCIONES ENVIADAS POR KERNEL
    crear_hilos_productor_consumidor();
    log_debug(logger, "Terminaron los hilos");

    // ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
    liberar_memoria();
    return 0;
}

void iniciar_listas_y_sem()
{
    lista_inst = list_create();
    l_FCBs_abiertos = list_create();


    pthread_mutex_init(&mutex_lista, NULL);
    sem_init(&cant_inst, 0, 0);
}

void listas_y_sem_destroy()
{
    list_destroy(lista_inst);
    list_destroy(l_FCBs_abiertos);

    pthread_mutex_destroy(&mutex_lista);
    sem_destroy(&cant_inst);
}

void crear_hilos_productor_consumidor()
{
    log_debug(logger, "Crando esquema productor consumidor");
    // Hilos Productor Consumidor
    iniciar_listas_y_sem();
    pthread_t hilo_productor, hilo_consumidor;
    
    pthread_create(&hilo_productor, NULL, (void *)crear_hilo_productor, NULL);
    pthread_create(&hilo_consumidor, NULL, (void *)crear_hilo_consumidor, NULL);

    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);

    listas_y_sem_destroy();
}

void liberar_memoria()
{
    // Liberando config
    config_destroy(config);
	free(configFS);
    // Liberando bitmap
    bitarray_destroy(bitA_bitmap);
    munmap(p_bitmap, stats_fd_bitmap.st_size);
}