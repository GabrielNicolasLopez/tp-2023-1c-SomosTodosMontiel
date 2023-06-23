#include "hilo_consumidor.h"

void crear_hilo_consumidor()
{
    t_instruccion* p_instruccion;
    while(1){
        
        sem_wait(&cant_inst);
        pthread_mutex_lock(&mutex_lista);
        p_instruccion = (t_instruccion*) list_remove(lista_inst, 0);
        pthread_mutex_unlock(&mutex_lista);


        // switch para procesar instruccion y para devolver paquete a kernel:
        // header: operacion
        // buffer: long_archivo y cadena_archivo

        int tipo_inst = p_instruccion->tipo;
        if (tipo_inst == F_OPEN) {
            if (existe_archivo(p_instruccion->cadena)) {
                t_lista_FCB_config* FCB;
                FCB->nombre_archivo = p_instruccion->cadena;
                FCB->config = buscar_FCB(p_instruccion->cadena);
                FCB->FCB_config = levantar_FCB(config);
                list_add(l_FCBs_abiertos, (void*) FCB);

                respuesta_a_kernel(FS_OPEN_OK, p_instruccion);
            }
            respuesta_a_kernel(FS_OPEN_NO_OK, p_instruccion);
        } else
        if (tipo_inst == F_CREATE) {
            t_lista_FCB_config* FCB;
            FCB->nombre_archivo = p_instruccion->cadena;
            FCB->config = crear_FCB(p_instruccion->cadena);
            FCB->FCB_config = levantar_FCB(config);
            list_add(l_FCBs_abiertos, (void*) FCB);

            respuesta_a_kernel(FS_CREATE_OK, p_instruccion);
        } else
        if (tipo_inst == F_TRUNCATE) {
            
            
            if (p_instruccion->paramIntA > p_instruccion->cadena);
        } else
        if (tipo_inst == F_READ) {
            
        } else
        if (tipo_inst == F_WRITE) {
            
        } else
        if (tipo_inst == EXIT) {
            break;
        }
    }
}

void respuesta_a_kernel(int operacion, t_instruccion* instruccion) 
{
    uint8_t header = operacion;
    t_buffer* buffer = buffer_create();

    // LONGITUD_ARCHIVO
    buffer_pack(buffer, &instruccion->longitud_cadena, sizeof(uint32_t));
	// CADENA_ARCHIVO
    buffer_pack(buffer, instruccion->cadena, instruccion->longitud_cadena);

    stream_send_buffer(socketKernel, header, buffer);
    buffer_destroy(buffer);
}
