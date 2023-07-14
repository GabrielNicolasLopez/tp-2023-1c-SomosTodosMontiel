#include "hilo_productor.h"

void crear_hilo_productor()
{
    t_instruccion_FS* p_instruccion;
    while(1){
        // REVISO SI HAY QUE FINALIZAR
        log_debug(logger, "Hilo_productor: esperando instruccion de kernel");
        uint8_t header = stream_recv_header(socketKernel);
        
        if ( header == (uint8_t) FS_FINALIZAR) {
            log_info(logger, "Hilo_productor: FS debe finalizar");
            // LE AVISO A HILO_CONSUMIDOR
            p_instruccion = malloc(sizeof(t_instruccion_FS));
            p_instruccion->tipo = EXIT;
            pthread_mutex_lock(&mutex_lista);
            list_add(lista_inst, (void*) p_instruccion);
            pthread_mutex_unlock(&mutex_lista);
            break;
        }
        
        if ( header != (uint8_t) FS_INSTRUCCION) {
            log_info(logger, "Hilo_productor: FS debe finalizar");
            // LE AVISO A HILO_CONSUMIDOR
            p_instruccion = malloc(sizeof(t_instruccion_FS));
            p_instruccion->tipo = EXIT;
            pthread_mutex_lock(&mutex_lista);
            list_add(lista_inst, (void*) p_instruccion);
            pthread_mutex_unlock(&mutex_lista);
            
            exit(-1);
        }

        p_instruccion = recibir_instruccion(socketKernel);
        
        pthread_mutex_lock(&mutex_lista);
        list_add(lista_inst, (void*) p_instruccion);
        pthread_mutex_unlock(&mutex_lista);
        sem_post(&cant_inst);
    }
}

t_instruccion_FS* recibir_instruccion(int socket)
{
    t_buffer* buffer = buffer_create();
    stream_recv_buffer(socket, buffer);
    log_debug(logger, "Tamaño de la instruccion recibida %u", buffer->size);

    t_instruccion_FS* instruccion = malloc(sizeof(t_instruccion_FS));
    //TIPO INSTRUCCION
    buffer_unpack(buffer, &instruccion->tipo, sizeof(t_tipoInstruccion));

    if(instruccion->tipo == F_OPEN){
        //Longitud cadena
        buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
        instruccion->cadena = malloc(instruccion->longitud_cadena);
        //Cadena
        buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);
    } else
    if(instruccion->tipo == F_CREATE){
        //Longitud cadena
        buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
        instruccion->cadena = malloc(instruccion->longitud_cadena);
        //Cadena
        buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);
    } else
    if(instruccion->tipo == F_TRUNCATE){
        //Longitud cadena
        buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
        instruccion->cadena = malloc(instruccion->longitud_cadena);
        //Cadena
        buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);
        //Parametro A
        buffer_unpack(buffer, &instruccion->paramIntA, sizeof(uint32_t));
    } else
    if(instruccion->tipo == F_WRITE){
        //Longitud cadena
        buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
        instruccion->cadena = malloc(instruccion->longitud_cadena);
        //Cadena
        buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);
        //Parametro A <------- Puntero del archivo
        buffer_unpack(buffer, &instruccion->paramIntA, sizeof(uint32_t));
        //Parametro B <------- Cant. Bytes a escribir
        buffer_unpack(buffer, &instruccion->paramIntB, sizeof(uint32_t));
        //Parametro C <------- Dir. Memoria Física
        buffer_unpack(buffer, &instruccion->paramIntC, sizeof(uint32_t));
        //PID
        buffer_pack(buffer, &instruccion->pid, sizeof(uint32_t));
    } else
    if(instruccion->tipo == F_READ){
        //Longitud cadena
        buffer_unpack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
        instruccion->cadena = malloc(instruccion->longitud_cadena);
        //Cadena
        buffer_unpack(buffer, instruccion->cadena, instruccion->longitud_cadena);
        //Parametro A <------- Puntero del archivo
        buffer_unpack(buffer, &instruccion->paramIntA, sizeof(uint32_t));
        //Parametro B <------- Cant. Bytes a leer
        buffer_unpack(buffer, &instruccion->paramIntB, sizeof(uint32_t));
        //Parametro C <------- Dir. Memoria Física
        buffer_unpack(buffer, &instruccion->paramIntC, sizeof(uint32_t));
        //PID
        buffer_pack(buffer, &instruccion->pid, sizeof(uint32_t));
    }
    
    buffer_destroy(buffer);

    return instruccion;
}