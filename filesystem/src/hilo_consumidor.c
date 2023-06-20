#include "hilo_consumidor.h"

void crear_hilo_consumidor()
{
    t_instruccion* p_instruccion;
    bool finalizar = false;
    while(1){
        
        sem_wait(&cant_inst);
        pthread_mutex_lock(&mutex_lista);
        p_instruccion = (t_instruccion*) list_remove(lista_inst, 0);
        pthread_mutex_unlock(&mutex_lista);

        // switch para procesar instruccion y para devolver paquete a kernel con header (tipos de devoluciones) y contenido (ej: tamaño - nombre Archivo)
        switch (p_instruccion->tipo)
        {
        case F_OPEN:
            /* code */
            break;
        case F_TRUNCATE:
            /* code */
            break;
        case F_READ:
            /* code */
            break;
        case F_WRITE:
            /* code */
            break;
        case EXIT:
            finalizar = true;
            break;
        default:
            break;
        }
        
        if (finalizar == true) {
            break;
        }
    }
}