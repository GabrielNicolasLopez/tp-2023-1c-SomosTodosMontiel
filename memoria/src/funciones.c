#include <funciones.h>
#include "algoritmos.h"

//Creación de Segmento

t_segmento* segmentoCrear(int pid,int id, int base, int tam){

    t_segmento* segmento=malloc(sizeof(t_segmento));
    segmento ->pid = pid;
    segmento ->id_segmento = id;
    segmento ->base = base;
    segmento ->tamanio = tam;
     
    return segmento;
}

void huecoCrear(t_segmento* segmento, t_hueco *hueco){
    hueco->base    = segmento->base;
    hueco->tamanio = segmento->tamanio;
}

uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio){
    t_hueco* hueco;
    uint32_t espacioLibre = 0;
    //Verificar disponibilidad de espacio contiguo
    for (int i = 0; i < list_size(listaHuecos); i++) {
        hueco = list_get(listaHuecos,i);
        if (hueco->tamanio>=tamanio) {
            log_info(logger,"Hay espacio suficiente para crear el segmento");
            return 1;
        }
    }
    
    //Verificar si es necesario solicitar compactación
    for (int i = 0; i < list_size(listaHuecos); i++) {
        hueco = list_get(listaHuecos,i);
        espacioLibre += hueco->tamanio;
        if (espacioLibre>=tamanio) //Necesito compactar
        {
            log_info(logger,"Se Necesita compactar");
            return -1;
        }    
    }
    //Informar falta de espacio libre al Kernel
    log_error(logger,"No hay espacio suficiente para crear el Segmento Out of Memory.");
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
        
        break;

    case BEST:
         direccionBase=algoritmoBestFit(tamSegmento);
        
        break;

    case WORST:
        direccionBase=algoritmoWorstFit(tamSegmento);
        
        break;
    
    default:
        log_error(logger, "No se eligio correctamente el algoritmo de memoria...");
        break;
    }
    return direccionBase;
}

t_tipo_algoritmo obtenerAlgoritmo(){

	char *algoritmoConfig = configuracionMemoria->algoritmo_asignacion;
	t_tipo_algoritmo algoritmoConfi;

	if(!strcmp(algoritmoConfig, "FIRST"))
		algoritmoConfi = FIRST;
	else if(!strcmp(algoritmoConfig, "BEST"))
		algoritmoConfi = BEST;
	else if(!strcmp(algoritmoConfig, "WORST"))
		algoritmoConfi = WORST;
        else
		log_error(logger, "ALGORITMO ESCRITO INCORRECTAMENTE");
	return algoritmoConfi;
}

t_segmento* buscarSegmentoPorIdPID(uint32_t id, uint32_t pid) {
    
    /* log_debug(logger, "cantidad de segmentos: %d", list_size(listaSegmentos)); */
    for (int i = 0; i < list_size(listaSegmentos); i++) {
        t_segmento* segmento = (t_segmento*)list_get(listaSegmentos, i);
        if (segmento->id_segmento == id && segmento->pid==pid) {
            log_debug(logger, "pid: %d, id: %d", pid, id);
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

void buscarSegmentoPorPID(t_list* listaABorrar, uint32_t pid){
    t_segmento* segmento;
    //Agregar el segmento 0 de primera
    for (int i = 0; i < list_size(listaSegmentos); i++) {
        segmento = malloc(sizeof(t_segmento));
        segmento = list_get(listaSegmentos, i);
        if (segmento->pid==pid) {
            list_add(listaABorrar, segmento); 
        }
    }
}

void eliminarProceso(t_list* listaSegmentosBorrar){
    t_hueco *huecoNuevo = malloc(sizeof(t_hueco));
    t_segmento* segmentoABorrar = malloc(sizeof(t_segmento));

    for (int i = 0; i <list_size(listaSegmentosBorrar); i++)
    {   
        segmentoABorrar = list_remove(listaSegmentosBorrar, 0);
        huecoCrear(segmentoABorrar, huecoNuevo);
        list_add(listaHuecos, huecoNuevo);
    }
}


//Compactar 
void compactar(){

    list_sort(listaSegmentos, compararPorBase);
    list_sort(listaHuecos, compararPorBase);
    uint32_t desplazamiento = 0;
    log_info(logger,"Se empezo a Compactar");
    for(int i=0;i<list_size(listaSegmentos);i++){
        t_segmento* segmento=list_get(listaSegmentos,i);
        segmento->base = desplazamiento;
        desplazamiento += segmento->tamanio;
    }

    //Borramos la lista de huecos vieja
    list_clean(listaHuecos);

    t_hueco* hueco_Nuevo = malloc(sizeof(t_hueco));
    hueco_Nuevo->base    = desplazamiento; 
    hueco_Nuevo->tamanio = configuracionMemoria->tam_memoria - desplazamiento;
    list_add(listaHuecos, hueco_Nuevo);
    log_info(logger,"Se termino de Compactar");
}

void buddySystem(){
    
    list_sort(listaHuecos, compararPorBase);
     
    t_hueco* huecoA = malloc(sizeof(t_hueco));
    t_hueco* huecoB = malloc(sizeof(t_hueco));
    uint32_t baseComparar;
    

    for (int i = 0; i < list_size(listaHuecos); i++)
    {
        if (i == list_size(listaHuecos) - 1){
            break;
        }
        huecoA = list_get(listaHuecos,i);
        huecoB = list_get(listaHuecos,i+1);
        
        baseComparar = huecoA->base + huecoA->tamanio;

        if(baseComparar == huecoB->base){

            huecoB->base=huecoA->base;
            huecoB->tamanio+=huecoA->tamanio;
            list_remove_element(listaHuecos,huecoA);
            
        }
    }
}