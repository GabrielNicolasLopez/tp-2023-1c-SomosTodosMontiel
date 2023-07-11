#include "comunicacion.h"


void hilo_kernel_m(){
    while (1){

        log_info(logger, "Estoy esperando mensaje de Kernel...");
        uint8_t header = stream_recv_header(conexion_con_kernel);
        log_info(logger, "header recibido: %d", header);

        switch (header){
            case INICIAR_PROCESO:
                //recibo un PID
                uint32_t pid_recibido = recibirPID();

                log_error(logger, "pid recibido: %d", pid_recibido);
                
                //te mando tamanio solo mando el tamanio
                mandarTam();

                log_info(logger,"Creación de Proceso PID: %d", pid_recibido);

                break;
            
            case FINALIZAR_PROCESO:
            //recibo uint32 pid
            
            uint32_t pid = recibirPID();
            t_list* listasABorrar = list_create();
            
            buscarSegmentoPorPID(listasABorrar, pid);

            //borro todos los segmentos con ese Pid y mando un PROCESO_BORRADO
            eliminarProceso(listasABorrar);
            mandarPrBr();

            log_info(logger,"Eliminación de Proceso PID: %d", pid);

            list_destroy(listasABorrar);

            break;

            case CREATE_SEGMENT:
                t_segmento *segmentoACrear = malloc(sizeof(t_segmento));

                //aca recibo PID ID TAM
                //recibirDatos(segmentoACrear->pid, segmentoACrear->id_segmento, segmentoACrear->tamanio);
                recibirDatos(segmentoACrear);
                //log_info(logger,"datos recibidos para crear un segmento: pid: %d, id: %d, tam: %d", segmentoACrear->pid, segmentoACrear->id_segmento, segmentoACrear->tamanio);
                
                //compruebo si hay espacio
                uint32_t espacioDisponible = comprobar_Creacion_de_Seg(segmentoACrear->tamanio);
                    
                //creo el segemnto y lo meto en la lista
                if(espacioDisponible==1){
                    segmentoACrear->base = aplicarAlgoritmo(segmentoACrear->tamanio);
                    
                    list_add(listaSegmentos, segmentoACrear);
                    log_info(logger,"PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d",segmentoACrear->pid,segmentoACrear->id_segmento,segmentoACrear->base,segmentoACrear->tamanio);
                }
                else{
                    segmentoACrear->base=espacioDisponible;
                    log_info(logger,"No se puedo crear el segmento  %d ",segmentoACrear->base);
                }
                //devuelvo la base actualizada
                mandarLaBase(segmentoACrear->base);

                break;

            case DELETE_SEGMENT:
                //recibo el segmento a eliminar 
                uint32_t id_recibido;
                uint32_t pid_recibido;

                recibirIDPID(id_recibido, pid_recibido);

                log_info(logger, "recibido. pid: %d, id: %d", pid_recibido, id_recibido);

                //elimino el segmento
                t_segmento *segmentoEncontrado = malloc(sizeof(t_segmento));
                segmentoEncontrado = buscarSegmentoPorIdPID(id_recibido, pid_recibido);

                t_hueco *huecoNuevo = malloc(sizeof(t_hueco));
                huecoNuevo = huecoCrear(segmentoEncontrado);
                //huecoNuevo->base =segmentoEncontrado->base;
                //huecoNuevo->tamanio=segmentoEncontrado->tamanio;
                list_add(listaHuecos,huecoNuevo);
                log_info(logger,"PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d",segmentoEncontrado->pid,segmentoEncontrado->id_segmento,segmentoEncontrado->base,segmentoEncontrado->tamanio);

                if(!list_remove_element(listaSegmentos,segmentoEncontrado)){
                    log_info(logger, "No lo puede remover");
                }   
                //devuelvo la lista nueva con el segemto elimado    
                t_list* listaMandar = list_create();
                buscarSegmentoPorPID(listaMandar, pid);
                mandarListaProceso(listaMandar); 
                break;
        

            case EMPEZA_A_COMPACTAR:

                log_info(logger, "Mi cod de op es: %d", header);
                
                log_info(logger, "Solicitud de compactacion");
                compactar();
                //aplicar retardospleep
                sleep((configuracionMemoria->retardo_compatacion/1000));
                for(int i;i<list_size(listaSegmentos);i++){
                    t_segmento* segmentoImprimir= list_get(listaSegmentos,i);
                    log_info(logger,"PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d",segmentoImprimir->pid,segmentoImprimir->id_segmento,segmentoImprimir->base,segmentoImprimir->tamanio);

                }
                //Recibo el header.#pragma endregionCompacto y devuelvo la lista actualizada    
                mandarListaGlobal(); //Falta terminar esto
                break;

            default:
                log_info(logger, "Instruccion no valida ERROR: %d", header);
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

void recibirIDPID(uint32_t id,uint32_t pid){

    t_buffer *buffer = buffer_create();

    stream_recv_buffer(conexion_con_kernel,buffer);

    buffer_unpack(buffer, &id, sizeof(uint32_t));

    buffer_unpack(buffer, &pid, sizeof(uint32_t));
    
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
    log_error(logger, "envie la base a kernel: %d", baseMandar);
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
    uint32_t tamanio_lista = list_size(listaSegmentos); //no seria el tamaño de lista?

    //Cantidad de segmentos 
    buffer_pack(buffer, &tamanio_lista, sizeof(uint32_t));

    //Segmentos
    for (int i = 0; i < tamanio_lista; i++)
    {   
        t_segmento *segmento = list_get(lista, i);
        buffer_pack(buffer,segmento,(sizeof(t_segmento)));
    }
    stream_send_buffer(conexion_con_kernel, LISTA, buffer);
    buffer_destroy(buffer);
}

void mandarListaGlobal(){

    t_buffer* buffer = buffer_create();
    uint32_t tamanio_lista =list_size(listaSegmentos);
    //buffer_pack(buffer,LISTA,sizeof(t_Kernel_Memoria)); // tipo de instruccion
    buffer_pack(buffer,&tamanio_lista,sizeof(uint32_t)); // tamaño de la lista de segmentos
    for (int i = 0; i < tamanio_lista; i++)
    {   
        t_segmento * segmento =list_get(listaSegmentos,i);  // saco el segmento de la posicion i
        buffer_pack(buffer,segmento,(sizeof(t_segmento)));  
    }
    stream_send_buffer(conexion_con_kernel, LISTA, buffer);
    buffer_destroy(buffer);
};