#include "hilo_consumidor.h"

void crear_hilo_consumidor()
{
    t_instruccion_FS* p_instruccion;
    while(1){
        
        sem_wait(&cant_inst);
        pthread_mutex_lock(&mutex_lista);
        p_instruccion = (t_instruccion_FS*) list_remove(lista_inst, 0);
        pthread_mutex_unlock(&mutex_lista);


        // switch para procesar instruccion y para devolver paquete a kernel:
        // header: operacion
        // buffer: long_archivo y cadena_archivo

        int tipo_inst = p_instruccion->tipo;

        log_debug(logger, "Hilo_consumidor: Instruccion recibida: %s", nombresInstrucciones[tipo_inst]);
        
        // *** F_OPEN ***
        if (tipo_inst == F_OPEN) {
            if (existe_archivo(p_instruccion->cadena)) {
                t_lista_FCB_config* FCB = malloc(sizeof(t_lista_FCB_config));
                FCB->nombre_archivo = p_instruccion->cadena;
                FCB->config = buscar_FCB(p_instruccion->cadena);
                FCB->FCB_config = levantar_FCB(FCB->config);
                list_add(l_FCBs_abiertos, (void*) FCB);
                
                log_info(logger, "Abrir Archivo: <%s>", p_instruccion->cadena);
                respuesta_a_kernel(FS_OPEN_OK, p_instruccion);
            } else {
                log_debug(logger, "Hilo_consumidor: Archivo inexistente, %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_OPEN_NO_OK, p_instruccion);
            }

        } else
        
        // *** F_CREATE ***
        if (tipo_inst == F_CREATE) {
            t_lista_FCB_config* FCB = malloc(sizeof(t_lista_FCB_config));
            FCB->nombre_archivo = p_instruccion->cadena;
            FCB->config = crear_FCB(p_instruccion->cadena);
            FCB->FCB_config = levantar_FCB(FCB->config);
            list_add(l_FCBs_abiertos, (void*) FCB);

            log_info(logger, "Crear Archivo: <%s>", p_instruccion->cadena);
            respuesta_a_kernel(FS_CREATE_OK, p_instruccion);

        } else
        
        if (tipo_inst == F_TRUNCATE) {
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);

            uint32_t tamanio_anterior = FCB->FCB_config->TAMANIO_ARCHIVO;
            uint32_t tamanio_nuevo = p_instruccion->paramIntA;
            
            if (tamanio_nuevo > tamanio_anterior) {
                // Asignar bloques: 
                asignar_bloques(FCB, tamanio_nuevo);
            } else {
                // Liberar bloques:
                liberar_bloques(FCB, tamanio_nuevo);
            }

            respuesta_a_kernel(FS_OK, p_instruccion);

        } else
        
        // *** F_READ ***
        if (tipo_inst == F_READ) {
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);
            
            uint8_t* cadena_bytes = leer_bloques(FCB, p_instruccion->paramIntA, p_instruccion->paramIntB); // DESARROLLAR!!
            if (!cadena_bytes) {
                log_error(logger, "Hilo_consumidor (F_READ): Error al leer los bloques del archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }

            pedido_escritura_mem(p_instruccion->paramIntB, cadena_bytes, p_instruccion->paramIntC);
            uint8_t header = stream_recv_header(socketMemoria);
            stream_recv_empty_buffer(socketMemoria);
            if (header != FS_M_WRITE_OK) {
                log_error(logger, "Hilo_consumidor (F_READ): Respuesta de escritura en memoria erronea. Archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            respuesta_a_kernel(FS_OK, p_instruccion);

        } else
        
        // *** F_WRITE ***
        if (tipo_inst == F_WRITE) {
            t_lista_FCB_config* FCB = FCB_list_get(p_instruccion->cadena);

            uint32_t puntero_archivo = p_instruccion->paramIntA;
            uint32_t cant_bytes = p_instruccion->paramIntB;
            uint32_t dir_fisica = p_instruccion->paramIntC;
            
            if (cant_bytes == 0) {
                log_debug(logger, "Hilo_consumidor (F_WRITE): La escritura es de 0 BYTES. Archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_OK, p_instruccion);
                continue;
            }
            
            // Puntero Cant. Bytes a escribir > TAM ARCHIVO
            if (puntero_archivo + cant_bytes > FCB->FCB_config->TAMANIO_ARCHIVO) {
                log_error(logger, "Hilo_consumidor (F_WRITE): La escritura supera el tamaño del archivo %s", p_instruccion->cadena);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
            
            pedido_lectura_mem(cant_bytes, dir_fisica);
            uint32_t cant_bytes_memoria;
            uint8_t* cadena_bytes; // dsp liberar
            recibir_cadena_bytes_mem(&cant_bytes_memoria, cadena_bytes);
            if (cant_bytes_memoria != cant_bytes) {
                log_error(logger, "Hilo_consumidor (F_WRITE): Memoria pudo leer solo %u bytes", cant_bytes_memoria);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }

            int bloques_escritos = escribir_bloques(FCB, puntero_archivo, cant_bytes, cadena_bytes); // DESARROLLAR!!
            if (bloques_escritos != cant_bytes) {
                log_error(logger, "Hilo_consumidor (F_WRITE): Solo se pudieron escribir en los bloques %u bytes", cant_bytes);
                respuesta_a_kernel(FS_ERROR, p_instruccion);
                continue;
            }
    
            respuesta_a_kernel(FS_OK, p_instruccion);
            free(cadena_bytes);

        } else
        
        // *** EXIT ***
        if (tipo_inst == EXIT) {
            log_info(logger, "Hilo_consumidor: Finalizacion exitosa");
            break;
        }
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

    stream_send_buffer(socketKernel, header, buffer);
    buffer_destroy(buffer);
}

void pedido_escritura_mem(uint32_t cantBytes, uint8_t* cadena_bytes, uint32_t dir_fisica)
{
    uint8_t header = FS_M_WRITE;
    t_buffer* buffer = buffer_create();

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_pack(buffer, &dir_fisica, sizeof(dir_fisica));
    // CANTIDAD DE BYTES
    buffer_pack(buffer, &cantBytes, sizeof(cantBytes));
	// CADENA DE BYTES
    buffer_pack(buffer, cadena_bytes, cantBytes);

    stream_send_buffer(socketMemoria, header, buffer);
    buffer_destroy(buffer);
}

void pedido_lectura_mem(uint32_t cantBytes, uint32_t dir_fisica)
{
    uint8_t header = FS_M_READ;
    t_buffer* buffer = buffer_create();

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_pack(buffer, &dir_fisica, sizeof(dir_fisica));
    // CANTIDAD DE BYTES
    buffer_pack(buffer, &cantBytes, sizeof(cantBytes));

    stream_send_buffer(socketMemoria, header, buffer);
    buffer_destroy(buffer);
}

void recibir_cadena_bytes_mem(uint32_t* cantBytes, uint8_t* cadena_bytes)
{
    t_buffer* buffer = buffer_create();
    stream_recv_buffer(socketMemoria, buffer);
    log_debug(logger, "Tamaño de la instruccion recibida %u", buffer->size);

    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(*cantBytes));
    cadena_bytes = malloc(*cantBytes);
    // CADENA DE BYTES
    buffer_unpack(buffer, cadena_bytes, *cantBytes);

    buffer_destroy(buffer);
}
