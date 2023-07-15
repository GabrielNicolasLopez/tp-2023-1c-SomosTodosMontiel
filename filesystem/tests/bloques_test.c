#include "filesystem.h"

#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

context (BLOQUES) {
    // LOGGER
    logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_INFO);
    if (logger == NULL) {
        fprintf(stderr, "Error al abrir el logger, abortando...");
        exit(EXIT_FAILURE);
    }
    
    // CONFIG
    config = config_create("./cfg/filesystem.cfg");
    configFS = leerConfiguracion(config);
    
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
            asignar_bloques(FCB, 1000);
        } end 

        after { 
            remove("/home/utnso/fs/fcb/IVAAAN");
            
            remove(configFS->PATH_BITMAP);
            remove(configFS->PATH_BLOQUES);
            remove(configFS->PATH_FCB);

            config_destroy(FCB->config);
            free(FCB);
        } end 
        /*
        it("Escribiendo y leyendo un solo bloque (n°5)") {
            uint32_t bloque = 10;
            off_t offset = 0;

            char* a_escribir = "0123456789ABCDEF";
            size_t tamanio = string_length(a_escribir);
            escribir_bloque(bloque, offset, a_escribir, tamanio);

            char* a_leer = malloc(tamanio + 1);
            leer_bloque(bloque, offset, a_leer, tamanio);
            a_leer[tamanio] = '\0';

            should_string(a_leer) be equal to(a_escribir);
        } end
        */
        
        it("Escritura y lectura de MULTIPLES bloques") {
            uint32_t puntero_archivo = 127;
            char* cadena_escrita = "HOLA COMO ESTAS?";
            uint32_t cant_bytes = string_length(cadena_escrita) + 1;
            escribir_bloques(FCB, puntero_archivo, cant_bytes, cadena_escrita);

            char* cadena_leida = leer_bloques(FCB, puntero_archivo, cant_bytes);
            
            should_string(cadena_leida) be equal to(cadena_escrita);
        } end
        
    } end 
}