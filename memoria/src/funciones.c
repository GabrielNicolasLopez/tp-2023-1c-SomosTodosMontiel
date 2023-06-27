#include <funciones.h>


//Creación de Segmento

t_segmento* segmentoCrear(int id, int base, int tam){

    t_segmento* segmento=malloc(sizeof(t_segmento));
    segmento ->id_segmento = id;
    segmento ->base = base;
    segmento ->tamanio = tam;
     
    return segmento;
}

uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio) {
    // Verificar disponibilidad de espacio contiguo
    bool espacioDisponible = 0;
    
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos,i);
        
        if (hueco->tamanio>=tamanio) {

            espacioDisponible = 1;
            return espacioDisponible;
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
    if (espacioDisponible || list_is_empty(listaHuecos)) {
        // Informar falta de espacio libre al Kernel
        return espacioDisponible;
    } 

    
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

t_segmento* buscarSegmentoPorId(uint32_t id) {
    for (int i = 0; i < list_size(listaSegmentos); i++) {
        t_segmento* segmento = (t_segmento*)list_get(listaSegmentos, i);
        if (segmento->id_segmento == id) {
            return segmento;
        }
    }
    return NULL;  // Si no se encuentra el segmento, se devuelve NULL
}
