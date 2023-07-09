#include <funciones.h>


//Creación de Segmento

t_segmento* segmentoCrear(int pid,int id, int base, int tam){

    t_segmento* segmento=malloc(sizeof(t_segmento));
    segmento ->pid = pid;
    segmento ->id_segmento = id;
    segmento ->base = base;
    segmento ->tamanio = tam;
     
    return segmento;
}

t_hueco* huecoCrear(t_segmento* segmento){

    t_hueco* hueco;
    hueco->base = segmento->base;
    hueco->tamanio= segmento->tamanio;

    return hueco;
}

uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio) {
    // Verificar disponibilidad de espacio contiguo
    
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos,i);
        
        if (hueco->tamanio>=tamanio) {
            
            return 1;
        }
    }
    
    // Verificar si es necesario solicitar compactación
    for (int i = 0; i < list_size(listaHuecos); i++) {
        uint32_t espacioLibre=0;
        t_hueco* hueco =list_get(listaHuecos,i);
        espacioLibre = espacioLibre + hueco->tamanio;
        if (espacioLibre>=tamanio)
        {
            //Necesito compactar
           return -1;
        }
                
    }
     // Informar falta de espacio libre al Kernel
        return 0;
     

    
}

uint32_t aplicarAlgoritmo(uint32_t tamSegmento){

t_tipo_algoritmo algoritmo;
algoritmo = obtenerAlgoritmo();
uint32_t direccionBase;

    switch (algoritmo)
    {
    case FIRST:
        direccionBase=algoritmoFirstFit(tamSegmento);
        return direccionBase;
        break;

    case BEST:
         direccionBase=algoritmoBestFit(tamSegmento);
        return direccionBase;
        break;

    case WORST:
        direccionBase=algoritmoWorstFit(tamSegmento);
        return direccionBase;
        break;
    
    default:
    //aca va un loger
        break;
    }


}

t_tipo_algoritmo obtenerAlgoritmo(){

	char *algoritmoConfig = configuracionMemoria->algoritmo_asignacion;
	t_tipo_algoritmo algoritmoConfi;

	if(!strcmp(algoritmoConfig, "FIRTS"))
		algoritmoConfi = FIRST;
	else if(!strcmp(algoritmoConfig, "BEST"))
		algoritmoConfi = BEST;
	else if(!strcmp(algoritmoConfig, "WORST"))
		algoritmoConfi = WORST;
        else
		log_error(logger, "ALGORITMO ESCRITO INCORRECTAMENTE");
	return algoritmoConfi;
}

t_segmento* buscarSegmentoPorIdPID(uint32_t id,uint32_t pid) {
    for (int i = 0; i < list_size(listaSegmentos); i++) {
        t_segmento* segmento = (t_segmento*)list_get(listaSegmentos, i);
        if (segmento->id_segmento == id && segmento->pid==pid) {
            return segmento;
        }
    }
    return NULL;  // Si no se encuentra el segmento, se devuelve NULL
}

uint32_t compararPorBase(const void* a, const void* b) {
    const t_segmento* segA = (const t_segmento*)a;
    const t_segmento* segB = (const t_segmento*)b;
    
    if (segA->base < segB->base)
        return -1;
    if (segA->base > segB->base)
        return 1;
    return 0;
}

t_list* buscarSegmentoPorPID(uint32_t pid){
    t_list* lista_Borrar = malloc(sizeof(t_list));
     t_segmento* segmento;
      for (int i = 0; i < list_size(listaSegmentos); i++) {

        segmento = list_get(listaSegmentos, i);

        if (segmento->pid==pid) {

            list_add(lista_Borrar,segmento); 
        }
    }
    return lista_Borrar;
}

void eliminarProceso(t_list* listaSegmentosBorrar){
        t_hueco * huecoNuevo;
        t_segmento* segmentoABorrar;

        for (int i = 0; i <list_size(listaSegmentosBorrar); i++)
        {   
            segmentoABorrar = lista_get(listaSegmentosBorrar,i);
            huecoNuevo =huecoCrear(segmentoABorrar);
            list_add(listaHuecos,huecoNuevo);
            list_remove_element(listaSegmentos,segmentoABorrar);
            
        }
        
}


//Compactar 

void compactar(){

    list_sort(listaSegmentos,compararPorBase);
    list_sort(listaHuecos,compararPorBase);
    uint32_t desplazamiento = 0;
    
    for(int i=0;i<list_size(listaSegmentos);i++){

        t_segmento* segmento=list_get(listaSegmentos,i);
        segmento->base = desplazamiento;
        desplazamiento += segmento->tamanio;

    }

   /* t_segmento* seg=list_get(listaSegmentos,list_size(listaSegmentos));
    desplazamiento = seg->tamanio;
    int tamHueco=0;
    
    for(int i=0;i<list_size(listaHuecos);i++){
        t_hueco *hueco =list_get(listaHuecos,i);
        tamHueco += hueco->tamanio;
    }
    */
    list_clean(listaHuecos);
    t_hueco* hueco_Nuevo;
    hueco_Nuevo ->base =desplazamiento; 
    hueco_Nuevo ->tamanio =configuracionMemoria->tam_memoria - desplazamiento;
    list_add(listaHuecos,hueco_Nuevo);

}

