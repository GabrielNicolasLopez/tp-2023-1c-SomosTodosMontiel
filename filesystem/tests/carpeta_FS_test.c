#include "filesystem.h"

#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

t_list* lista_inst;
t_list* l_FCBs_abiertos;

pthread_mutex_t mutex_lista;
sem_t cant_inst;

context (CARPETA_FS) {
    // LOGGER
    logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);
    if (logger == NULL) {
        fprintf(stderr, "Error al abrir el logger, abortando...");
        exit(EXIT_FAILURE);
    }
    
    // CONFIG
    config = config_create("./cfg/filesystem.cfg");
    configFS = leerConfiguracion(config);
    
    describe("TRUNCARAMIENTO DE ARCHIVOS") { 

        char* archivo = "IVAAAN";
        t_lista_FCB_config* FCB = NULL;

        before { 
            // LEVANTO ARCHIVOS DEL VOLUMEN (CARPETA FS)
            levantar_volumen();
            
            // CREO UN FCB
            FCB = malloc(sizeof(t_lista_FCB_config));
            FCB->nombre_archivo = archivo;
            FCB->config = crear_FCB(archivo);
            FCB->FCB_config = levantar_FCB(FCB->config);
        } end 

        after { 
            remove("/home/utnso/fs/fcb/IVAAAN");
            
            remove(configFS->PATH_BITMAP);
            remove(configFS->PATH_BLOQUES);
            remove(configFS->PATH_FCB);

            config_destroy(FCB->config);
            free(FCB);
        } end 

        it("Tamaño FCB al truncar para arriba (asignar bloques)") {
            asignar_bloques(FCB, 100);
            
            should_int(FCB->FCB_config->TAMANIO_ARCHIVO) be equal to(100);
        } end

        it("Bloques ocupados al truncar para arriba (asignar bloques)") {
            int bloques_ocupados = 0; 
            
            asignar_bloques(FCB, 100);

            for (int i = 0; i < bitarray_get_max_bit(bitA_bitmap); i++) {
                if (bitarray_test_bit(bitA_bitmap, i)) {
                    bloques_ocupados++;
                }
            }
            
            should_int(bloques_ocupados) be equal to(3);
        } end

        it("Tamaño FCB al truncar para abajo (liberar bloques)") {
            asignar_bloques(FCB, 101);
            liberar_bloques(FCB, 80);

            should_int(FCB->FCB_config->TAMANIO_ARCHIVO) be equal to(80);
        } end

        it("Bloques ocupados al truncar para abajo (liberar bloques)") {
            int bloques_ocupados = 0; 
            
            asignar_bloques(FCB, 100);
            liberar_bloques(FCB, 40);
            
            for (int i = 0; i < bitarray_get_max_bit(bitA_bitmap); i++) {
                if (bitarray_test_bit(bitA_bitmap, i)) {
                    bloques_ocupados++;
                }
            }
            
            should_int(bloques_ocupados) be equal to(1);
        } end
    } end 

    describe("ESCRITURA Y LECTURA") { 
        char* archivo = "IVAAAN";
        t_lista_FCB_config* FCB = NULL;

        before { 
            // LEVANTO ARCHIVOS DEL VOLUMEN (CARPETA FS)
            levantar_volumen();
            
            // CREO UN FCB
            FCB = malloc(sizeof(t_lista_FCB_config));
            FCB->nombre_archivo = archivo;
            FCB->config = crear_FCB(archivo);
            FCB->FCB_config = levantar_FCB(FCB->config);

            // LE ASIGNO UN TAMAÑO DE 100
            asignar_bloques(FCB, 100);
        } end 

        after { 
            remove("/home/utnso/fs/fcb/IVAAAN");
            
            remove(configFS->PATH_BITMAP);
            remove(configFS->PATH_BLOQUES);
            remove(configFS->PATH_FCB);

            config_destroy(FCB->config);
            free(FCB);
        } end 

        /*it("Escritura y lectura de bloques") {
            uint32_t puntero_archivo = 50;
            uint8_t* cadena_escrita = "HOLA TODO BIEN";
            uint32_t cant_bytes = string_length(cadena_escrita);
            escribir_bloques(FCB, puntero_archivo, cant_bytes, cadena_escrita);

            uint8_t* cadena_leida = leer_bloques(FCB, puntero_archivo, cant_bytes);
            
            should_string((char*)cadena_leida) be equal to((char*)cadena_escrita);
        } end
        */


    } end 


}