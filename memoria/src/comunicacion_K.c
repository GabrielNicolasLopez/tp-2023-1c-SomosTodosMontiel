#include "comunicacion.h"
#include "funciones.h"

void hilo_kernel_m(){
    uint32_t pid_recibido;
    while (1){

        log_info(logger, "Estoy esperando mensaje de Kernel...");
        uint8_t header = stream_recv_header(conexion_con_kernel);
        log_info(logger, "header recibido: %d", header);

        t_segmento* segmentoImprimir;
        switch (header){
            case INICIAR_PROCESO:
                //recibo un PID
                pid_recibido = recibirPID();

                log_debug(logger, "pid recibido: %d", pid_recibido);
                
                //te mando tamanio solo mando el tamanio
                mandarTam();

                log_info(logger,"Creación de Proceso PID: %d", pid_recibido);//obligatorio 

                break;
            
            case FINALIZAR_PROCESO:
            //recibo uint32 pid
            
            pid_recibido = recibirPID();
            t_list* listasABorrar = list_create();
            
            log_debug(logger, "pid recibido: %d", pid_recibido);
        
            buscarSegmentoPorPID(listasABorrar, pid_recibido);

            log_debug(logger,"Segmentos del Proceso PID:%d a borrar:",pid_recibido);
            for(int i=0; i < list_size(listasABorrar); i++){
                    segmentoImprimir = list_get(listasABorrar, i);
                    log_debug(logger,"Segmento: %d - Base: %d - Tamaño: %d", segmentoImprimir->id_segmento, segmentoImprimir->base, segmentoImprimir->tamanio);
                }

            //borro todos los segmentos con ese Pid y mando un PROCESO_BORRADO
            eliminarProceso(listasABorrar);
            buddySystem();
            mandarPrBr();

            log_info(logger,"Eliminación de Proceso PID: %d", pid_recibido);//obligatorio 

            list_destroy(listasABorrar);

            break;

            case CREATE_SEGMENT:
                t_segmento *segmentoACrear = malloc(sizeof(t_segmento));

                recibirDatos(segmentoACrear);

                log_debug(logger,"Datos recibidos para crear un segmento: pid: %d, id: %d, tam: %d", segmentoACrear->pid, segmentoACrear->id_segmento, segmentoACrear->tamanio);
                
                //compruebo si hay espacio
                uint32_t espacioDisponible = comprobar_Creacion_de_Seg(segmentoACrear->tamanio);
                    
                //creo el segemnto y lo meto en la lista
                if(espacioDisponible==1){
                   /*  log_debug(logger, "aplicando algoritmo..."); */
                    segmentoACrear->base = aplicarAlgoritmo(segmentoACrear->tamanio);
                    
                    list_add(listaSegmentos, segmentoACrear);
                    log_debug(logger, "Cantidad de segmentos en la lista: %d", list_size(listaSegmentos));
                    log_info(logger,"PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d",segmentoACrear->pid,segmentoACrear->id_segmento,segmentoACrear->base,segmentoACrear->tamanio);
                }
                else{
                    segmentoACrear->base=espacioDisponible;
                    log_error(logger,"No se puedo crear el segmento  %d ",segmentoACrear->base);
                }
                //devuelvo la base actualizada
                mandarLaBase(segmentoACrear->base);

                //free(segmentoACrear);

                break;

            case DELETE_SEGMENT:
                //recibo el segmento a eliminar 
                uint32_t *id = malloc(sizeof(uint32_t));
                uint32_t *pid = malloc(sizeof(uint32_t));

                recibirIDPID(id, pid);

                log_debug(logger, "Datos recibidos para eliminar segmento pid: %d, id: %d", *pid, *id);

                //elimino el segmento
                t_segmento *segmentoEncontrado = malloc(sizeof(t_segmento));
                t_hueco    *huecoNuevo         = malloc(sizeof(t_hueco));
                
                segmentoEncontrado = buscarSegmentoPorIdPID(*id, *pid);
                log_debug(logger, "tam: %d, pid: %d, id: %d, base: %d", segmentoEncontrado->tamanio, segmentoEncontrado->pid, segmentoEncontrado->id_segmento, segmentoEncontrado->base);
                huecoCrear(segmentoEncontrado, huecoNuevo);

                list_add(listaHuecos,huecoNuevo);
                
                log_info(logger,"PID: %d - Eliminar Segmento: %d - Base: %d - TAMAÑO: %d",segmentoEncontrado->pid,segmentoEncontrado->id_segmento,segmentoEncontrado->base,segmentoEncontrado->tamanio);

                log_debug(logger, "Cantidad de segmentos antes de borrar: %d", list_size(listaSegmentos));
                if(!list_remove_element(listaSegmentos,segmentoEncontrado)){
                    log_error(logger, "No lo puede remover");
                } 
                buddySystem();
                
                log_debug(logger, "Cantidad de segmentos despues de borrar: %d", list_size(listaSegmentos));
                //devuelvo la lista nueva con el segemto elimado    
                t_list* listaMandar = list_create();
                buscarSegmentoPorPID(listaMandar, *pid);
                list_add(listaMandar, segmento_0);
                mandarListaProceso(listaMandar); 
                
                list_destroy(listaMandar);
                // free(segmentoEncontrado);
                // free(huecoNuevo);
                // free(id);
                // free(pid);
                break;
        
            case EMPEZA_A_COMPACTAR:
                
                log_info(logger, "Solicitud de compactacion");
                compactar();
                sleep((configuracionMemoria->retardo_compatacion/1000));
                log_info(logger,"Resultado Compactación: ");
                for(int i=0; i < list_size(listaSegmentos); i++){
                    segmentoImprimir = list_get(listaSegmentos, i);
                    log_info(logger,"PID: %d - Segmento: %d - Base: %d - Tamaño: %d", segmentoImprimir->pid, segmentoImprimir->id_segmento, segmentoImprimir->base, segmentoImprimir->tamanio);
                }
                //Se envia la lista de segmentos global
                mandarListaProceso(listaSegmentos);
                break;

            default:
                log_error(logger, "Instruccion no valida ERROR: %d", header);
                break;
        }
        //free(header);
    }
}


//Recibir
void recibirDatos(t_segmento *segmentoACrear){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_kernel, buffer);

    //PID del segmento a crear
    buffer_unpack(buffer, &segmentoACrear->pid, sizeof(uint32_t));
    
    //ID del segmento a crear
    buffer_unpack(buffer, &segmentoACrear->id_segmento, sizeof(uint32_t));
    
    //Tamaño del segmento a crear
	buffer_unpack(buffer, &segmentoACrear->tamanio, sizeof(uint32_t));

    buffer_destroy(buffer);
}

void recibirIDPID(uint32_t *id, uint32_t *pid){

    t_buffer *buffer = buffer_create();

    //uint32_t var_id, var_pid;

    stream_recv_buffer(conexion_con_kernel,buffer);

    buffer_unpack(buffer, id, sizeof(uint32_t));

    buffer_unpack(buffer, pid, sizeof(uint32_t));

    // *id = var_id;
    
    // *pid = var_pid;

    buffer_destroy(buffer);
}

uint32_t recibirPID(){

    t_buffer *buffer = buffer_create();
    uint32_t pid_recibido;

    stream_recv_buffer(conexion_con_kernel, buffer);

    buffer_unpack(buffer, &pid_recibido, sizeof(uint32_t));

    buffer_destroy(buffer);

    return pid_recibido;
}

//Mandar

void mandarLaBase(uint32_t baseMandar){

    t_buffer* buffer = buffer_create();

    t_Kernel_Memoria instruccion;

    if(baseMandar != 0 && baseMandar != -1){
        instruccion = BASE;
        buffer_pack(buffer, &baseMandar, sizeof(uint32_t));
    }
    if(baseMandar == 0 )
        instruccion = SIN_MEMORIA;
    if(baseMandar == -1 )
        instruccion = NECESITO_COMPACTAR;

    stream_send_buffer(conexion_con_kernel, instruccion, buffer);
    /* log_debug(logger, "envie la base a kernel: %d", baseMandar); */
    buffer_destroy(buffer);
}

void mandarTam(){
    t_buffer* buffer = buffer_create();
    //log_error(logger, "tam_segm0: %d", configuracionMemoria->tam_segmento_O);
    buffer_pack(buffer, &(configuracionMemoria->tam_segmento_O), sizeof(uint32_t));
    stream_send_buffer(conexion_con_kernel, TAMANIO, buffer);
    buffer_destroy(buffer);
}

void mandarPrBr(){
    t_buffer* buffer = buffer_create();
    //buffer_pack(buffer, PROCESO_BORRADO, sizeof(t_Kernel_Memoria));
    //stream_send_buffer(conexion_con_kernel, PROCESO_BORRADO, buffer);
    stream_send_empty_buffer(conexion_con_kernel, PROCESO_BORRADO);
    buffer_destroy(buffer);
}

void mandarListaProceso(t_list *lista){

    t_buffer* buffer = buffer_create();
    uint32_t tamanio_lista = list_size(lista);

    //Cantidad de segmentos 
    buffer_pack(buffer, &tamanio_lista, sizeof(uint32_t));

    //Segmentos
    t_segmento *segmento;
    /* log_debug(logger, "cantidad de segmentos al mandar: %d", tamanio_lista); */
    for (int i = 0; i < tamanio_lista; i++)
    {   
        segmento = list_get(lista, i);
        buffer_pack(buffer, &(segmento->pid),         sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->id_segmento), sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->base),        sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->tamanio),     sizeof(uint32_t));
        log_debug(logger, "pid: %d, id: %d, base: %d, tam: %d",	segmento->pid, segmento->id_segmento, segmento->base, segmento->tamanio);
    }

    stream_send_buffer(conexion_con_kernel, LISTA, buffer);
  /*   log_debug(logger, "envie la lista a kernel: %d", buffer->size); */
    buffer_destroy(buffer);
}

