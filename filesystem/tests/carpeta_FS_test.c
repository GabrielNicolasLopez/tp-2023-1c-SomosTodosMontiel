#include "filesystem.h"

#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

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
            
            asignar_bloques(FCB, 1000);

            for (int i = 0; i < bitarray_get_max_bit(bitA_bitmap); i++) {
                if (bitarray_test_bit(bitA_bitmap, i)) {
                    bloques_ocupados++;
                }
            }
            
            should_int(bloques_ocupados) be equal to(17);
        } end

        it("Tamaño FCB al truncar para abajo (liberar bloques)") {
            asignar_bloques(FCB, 101);
            liberar_bloques(FCB, 10);

            should_int(FCB->FCB_config->TAMANIO_ARCHIVO) be equal to(10);
        } end

        it("Bloques ocupados al truncar para abajo (liberar bloques)") {
            int bloques_ocupados = 0; 
            
            asignar_bloques(FCB, 101);
            liberar_bloques(FCB, 10);
            
            for (int i = 0; i < bitarray_get_max_bit(bitA_bitmap); i++) {
                if (bitarray_test_bit(bitA_bitmap, i)) {
                    bloques_ocupados++;
                }
            }
            
            should_int(bloques_ocupados) be equal to(1);
        } end
    } end 
}