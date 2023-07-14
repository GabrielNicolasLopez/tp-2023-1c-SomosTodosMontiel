#include "hilo_consumidor.h"

void crear_hilo_consumidor()
{
    while(1){
        t_instruccion_FS* p_instruccion;
        sem_wait(&cant_inst);
        pthread_mutex_lock(&mutex_lista);
        p_instruccion = (t_instruccion_FS*) list_remove(lista_inst, 0);
        pthread_mutex_unlock(&mutex_lista);

        int tipo_inst = p_instruccion->tipo;
        log_debug(logger, "Hilo_consumidor: Instruccion recibida: %s", nombresInstrucciones[tipo_inst]);

        // *** F_OPEN ***
        if (tipo_inst == F_OPEN) {
            if (existe_FCB(p_instruccion->cadena)) {
                log_info(logger, "Abrir Archivo: <%s>", p_instruccion->cadena);
                
                t_lista_FCB_config* FCB = malloc(sizeof(t_lista_FCB_config));
                FCB->nombre_archivo = p_instruccion->cadena;
                FCB->config = buscar_FCB(p_instruccion->cadena);
                FCB->FCB_config = levantar_FCB(FCB->config);
                list_add(l_FCBs_abiertos, (void*) FCB);
                
                respuesta_a_kernel(FS_OPEN_OK, p_instruccion);
            } else {
                log_info(logger, "F_OPEN: Archivo inexistente, %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_OPEN_NO_OK, p_instruccion);
                free(p_instruccion->cadena);
            }

        } else
        
        // *** F_CREATE ***
        if (tipo_inst == F_CREATE) {
            log_info(logger, "Crear Archivo: <%s>", p_instruccion->cadena);
            
            t_lista_FCB_config* FCB = malloc(sizeof(t_lista_FCB_config));
            char* archivo = strdup(p_instruccion->cadena);
            FCB->nombre_archivo = archivo;
            FCB->config = crear_FCB(archivo);
            FCB->FCB_config = levantar_FCB(FCB->config);
            list_add(l_FCBs_abiertos, (void*) FCB);

            respuesta_a_kernel(FS_CREATE_OK, p_instruccion);
            free(p_instruccion->cadena);

        } else
        
        if (tipo_inst == F_TRUNCATE) {
            log_info(logger, "Truncar Archivo: <%s> - Tamaño: <%u>", p_instruccion->cadena, p_instruccion->paramIntA);
            
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);

            uint32_t tamanio_anterior = FCB->FCB_config->TAMANIO_ARCHIVO;
            uint32_t tamanio_nuevo = p_instruccion->paramIntA;
            
            if (tamanio_nuevo > tamanio_anterior) {
                // Asignar bloques: 
                asignar_bloques(FCB, tamanio_nuevo);
            } else 
            if (tamanio_nuevo < tamanio_anterior) {
                // Liberar bloques:
                liberar_bloques(FCB, tamanio_nuevo);
            }

            respuesta_a_kernel(FS_OK, p_instruccion);
            free(p_instruccion->cadena);

        } else
        
        // *** F_READ ***
        if (tipo_inst == F_READ) {
            log_info(logger, "ENTRE A F_READ");
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);
            
            uint32_t puntero_archivo = p_instruccion->paramIntA;
            uint32_t cant_bytes = p_instruccion->paramIntB;
            uint32_t dir_fisica = p_instruccion->paramIntC;
            
            if (cant_bytes == 0) {
                log_debug(logger, "Hilo_consumidor (F_READ): La lectura es de 0 BYTES. Archivo %s", p_instruccion->cadena);
                log_info(logger, "Leer Archivo: <%s> - Puntero: <%u> - Memoria: <%u> - Tamaño: <%u>",
                    p_instruccion->cadena, 
                    puntero_archivo,
                    dir_fisica,
                    cant_bytes
                );
                respuesta_a_kernel(FS_OK, p_instruccion);
                free(p_instruccion->cadena);
                continue;
            }
            
            if (puntero_archivo + cant_bytes > FCB->FCB_config->TAMANIO_ARCHIVO) {
                log_error(logger, "Hilo_consumidor (F_READ): La lectura supera el tamaño del archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            log_info(logger, "Leer Archivo: <%s> - Puntero: <%u> - Memoria: <%u> - Tamaño: <%u>",
                p_instruccion->cadena, 
                puntero_archivo,
                dir_fisica,
                cant_bytes
            );
            char* cadena_bytes = leer_bloques(FCB, puntero_archivo, cant_bytes);

            pedido_escritura_mem(cant_bytes, cadena_bytes, dir_fisica, p_instruccion->pid);
            uint8_t header = stream_recv_header(socketMemoria);
            stream_recv_empty_buffer(socketMemoria);
            if (header != FS_M_WRITE_OK) {
                log_error(logger, "Hilo_consumidor (F_READ): Respuesta de escritura en memoria erronea. Archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            respuesta_a_kernel(FS_OK, p_instruccion);
            free(cadena_bytes);
            free(p_instruccion->cadena);

        } else
        
        // *** F_WRITE ***
        if (tipo_inst == F_WRITE) {
            log_info(logger, "ENTRE A F_WRITE");
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);

            uint32_t puntero_archivo = p_instruccion->paramIntA;
            uint32_t cant_bytes = p_instruccion->paramIntB;
            uint32_t dir_fisica = p_instruccion->paramIntC;
            
            if (cant_bytes == 0) {
                log_debug(logger, "Hilo_consumidor (F_WRITE): La escritura es de 0 BYTES. Archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_OK, p_instruccion);
                //free_p_instruccion(p_instruccion);
                continue;
            }

            if (puntero_archivo + cant_bytes > FCB->FCB_config->TAMANIO_ARCHIVO) {
                log_error(logger, "Hilo_consumidor (F_WRITE): La escritura supera el tamaño del archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            pedido_lectura_mem(cant_bytes, dir_fisica, p_instruccion->pid);
            uint8_t header = stream_recv_header(socketMemoria);
            if (header != FS_M_READ_OK) {
                log_error(logger, "Hilo_consumidor (F_READ): Respuesta de escritura en memoria erronea. Archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            uint32_t cant_bytes_memoria;
            char* cadena_bytes = recibir_cadena_bytes_mem(&cant_bytes_memoria);
            
            if (cant_bytes_memoria != cant_bytes) {
                log_error(logger, "Hilo_consumidor (F_WRITE): Memoria pudo leer solo %u bytes", cant_bytes_memoria);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                free(cadena_bytes);
                continue;
            }

            log_info(logger, "Escribir Archivo: <%s> - Puntero: <%u> - Memoria: <%u> - Tamaño: <%u>",
                p_instruccion->cadena, 
                puntero_archivo,
                dir_fisica,
                cant_bytes
            );
            int bloques_escritos = escribir_bloques(FCB, puntero_archivo, cant_bytes, cadena_bytes);
            if (bloques_escritos != cant_bytes) {
                log_error(logger, "Hilo_consumidor (F_WRITE): Solo se pudieron escribir en los bloques %u bytes", cant_bytes);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                free(cadena_bytes);
                continue;
            }
            respuesta_a_kernel(FS_OK, p_instruccion);
            free(cadena_bytes);
            free(p_instruccion->cadena);

        } else
        
        // *** EXIT ***
        if (tipo_inst == EXIT) {
            log_info(logger, "Hilo_consumidor: Finalizacion exitosa");
            break;
        }
        
        free(p_instruccion);
    }
}



void respuesta_a_kernel(int operacion, t_instruccion_FS* instruccion) 
{
    uint8_t header = operacion;
    t_buffer* buffer = buffer_create();

    // LONGITUD_ARCHIVO
    buffer_pack(buffer, &instruccion->longitud_cadena, sizeof(instruccion->longitud_cadena));
	// CADENA_ARCHIVO
    buffer_pack(buffer, instruccion->cadena, instruccion->longitud_cadena);
    log_error(logger, "Tamaño de la instruccion enviada a MEMORIA %d", buffer->size);
    stream_send_buffer(socketKernel, header, buffer);
    buffer_destroy(buffer);
}

void pedido_escritura_mem(uint32_t cantBytes, char* cadena_bytes, uint32_t dir_fisica, uint32_t pid)
{
    uint8_t header = FS_M_WRITE;
    t_buffer* buffer = buffer_create();

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_pack(buffer, &dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_pack(buffer, &cantBytes, sizeof(uint32_t));
	// CADENA DE BYTES
    buffer_pack(buffer, cadena_bytes, cantBytes);
    //PID
    buffer_pack(buffer, &pid, sizeof(uint32_t));

    stream_send_buffer(socketMemoria, header, buffer);
    buffer_destroy(buffer);
}

void pedido_lectura_mem(uint32_t cantBytes, uint32_t dir_fisica, uint32_t pid)
{
    uint8_t header = FS_M_READ;
    t_buffer* buffer = buffer_create();

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_pack(buffer, &dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_pack(buffer, &cantBytes, sizeof(uint32_t));
    //PID
    buffer_pack(buffer, &pid, sizeof(uint32_t));

    stream_send_buffer(socketMemoria, header, buffer);
    buffer_destroy(buffer);
}

char* recibir_cadena_bytes_mem(uint32_t* cantBytes)
{
    t_buffer* buffer = buffer_create();
    stream_recv_buffer(socketMemoria, buffer);
    log_debug(logger, "Tamaño de la instruccion recibida %u", buffer->size);

    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(*cantBytes));
    // CADENA DE BYTES
    char* cadena_bytes = malloc(*cantBytes);
    buffer_unpack(buffer, cadena_bytes, *cantBytes);

    buffer_destroy(buffer);
    return cadena_bytes;
}
