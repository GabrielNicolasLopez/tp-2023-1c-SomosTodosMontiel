#include <comunicacion_K.h>


void hilo_kernel()
{ while (1){
    log_info(logger, "Estoy esperando paquete, soy memoria\n");
	
        uint8_t header = stream_recv_header(conexion_con_kernel);

        switch (header)
        {
        case CREATE_SEGMENT:
            log_info(logger, "Mi cod de op es: %d", header);
            t_segmento *segmentoACrear=malloc(sizeof(t_segmento));
            uint32_t baseMandar;

        //-aca recibo el tamanio del segmento, la id
            segmentoACrear =recibirIDTam(segmentoACrear->id_segmento,segmentoACrear->tamanio);
            
        //compruebo si hay espacio
            uint32_t espacioDisponible =comprobar_Creacion_de_Seg(segmentoACrear->tamanio);
            
        //-creo el segemnto y lo meto en la lista  
            if(espacioDisponible==1){
                baseMandar=aplicarAlgoritmo(segmentoACrear->tamanio);
                t_segmento* segmento = segmentoCrear(segmentoACrear->id_segmento,baseMandar,segmentoACrear->tamanio);
                list_add(listaSegmentos,segmento);
            }
            else{
                baseMandar=espacioDisponible;
            }
        //-devuelvo la base actualizada
            mandarLaBase(baseMandar);

            break;

        case DELETE_SEGMENT:

            log_info(logger, "Mi cod de op es: %d", header);

        //recibo el segmento a eliminar 
            uint32_t id;
            uint32_t id_recivida = recibirID(id);
        //elimino el segmento
            t_segmento *segmentoEncontrado;
            segmentoEncontrado=buscarSegmentoPorId(id_recivida);
            t_hueco * huecoNuevo=malloc(sizeof(t_hueco));
            huecoNuevo->base =segmentoEncontrado->base;
            huecoNuevo->tamanio=segmentoEncontrado->tamanio;
            list_add(listaHuecos,huecoNuevo);
            list_remove_element(listaSegmentos,segmentoEncontrado);
        //devuelvo la lista nueva con el segemto elimado     
        // mandarLista(); Falta terminar esto 
            
            break;
     

        case EMPEZA_A_COMPACTAR:

            log_info(logger, "Mi cod de op es: %d", header);
            compactar();
            //Recibo el header.#pragma endregionCompacto y devuelvo la lista actualizada    
            
            
            break;
        default:
            log_info(logger, "Instruccion no valida ERROR: %d", header);

            break;
        }
        free(header);
    }
    
}

t_segmento *recibirIDTam(uint32_t id, uint32_t tam){

    t_buffer* buffer = buffer_create();

     stream_recv_buffer(conexion_con_kernel,buffer);

    t_segmento *seg =malloc(sizeof(t_segmento));

    //ID del segmento a crear
    seg->id_segmento=buffer_unpack(buffer,&id,sizeof(uint32_t));
    
    //Tamaño del segmento a crear
	seg->tamanio=buffer_unpack(buffer, &tam, sizeof(uint32_t));

    buffer_destroy(buffer);

    return seg;

}

void mandarLaBase(uint32_t baseMandar){

    t_buffer* buffer = buffer_create();

    t_Kernel_Memoria instruccion;

    if(baseMandar !=0 && baseMandar!=-1){
        instruccion = BASE;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        buffer_pack(buffer,&baseMandar,sizeof(uint32_t));
    }
    if(baseMandar == 0 ){
        instruccion = SIN_MEMORIA;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        //stream_send_buffer(socketKernel, &instruccion);
    }
    if(baseMandar == -1 ){
        instruccion = NECESITO_COMPACTAR;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        //stream_send_buffer(socketKernel, &instruccion);
    }

    stream_send_buffer(conexion_con_kernel, &instruccion, buffer);
    buffer_destroy(buffer);
}


uint32_t recibirID(uint32_t id){

    t_buffer *buffer =buffer_create();

    uint32_t id_recibida;

    stream_recv_buffer(conexion_con_kernel,buffer);

    id_recibida=buffer_unpack(buffer,&id,sizeof(uint32_t));
    buffer_destroy(buffer);

    return id_recibida;


}