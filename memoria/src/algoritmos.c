#include <algoritmos.h>



//FirstFit
uint32_t asignarSegmentoFirstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento) {

    // Buscar el primer hueco que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        if (hueco->tamanio >= tamSegmento) {
            // Asignar el segmento
            t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
            nuevoSegmento->id_segmento= idProceso;
            nuevoSegmento->base = hueco->base;
            nuevoSegmento->tamanio = tamSegmento;

            // Actualizar el hueco libre
            hueco->base += tamSegmento;
            hueco->tamanio -= tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return nuevoSegmento->base;
        }
    }

     
}

//BestFit

void asignarSegmentoBestFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento) {
    // Ordenar los huecos por tamaño de forma ascendente
    list_sort(listaHuecos, compararHuecosPorTamanioAscendente);

    // Buscar el hueco más pequeño que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        if (hueco->tamanio >= tamSegmento) {
            // Asignar el segmento
            t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
            nuevoSegmento->id_segmento= idProceso;
            nuevoSegmento->base = hueco->base;
            nuevoSegmento->tamanio = tamSegmento;
            //list_add(listaSegmentos, nuevoSegmento);

            // Actualizar el hueco libre
            hueco->base += tamSegmento;
            hueco->tamanio -= tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return nuevoSegmento->base;
        }
    }

    
}


//Worts Fit

void asignarSegmentoWorstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento) {
    // Ordenar los huecos por tamaño de forma descendente
    list_sort(listaHuecos, compararHuecosPorTamanioDescendente);

    // Buscar el hueco más grande que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        if (hueco->tamanio >= tamSegmento) {
            // Asignar el segmento
            t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
            nuevoSegmento->id_segmento= idProceso;
            nuevoSegmento->base = hueco->base;
            nuevoSegmento->tamanio = tamSegmento;
            //list_add(listaSegmentos, nuevoSegmento);

            // Actualizar el hueco libre
            hueco->base += tamSegmento;
            hueco->tamanio -= tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return nuevoSegmento->base;
        }
    }
    
}

int compararHuecosPorTamanioAscendente(const void* a, const void* b) {
    t_hueco* huecoA = *(t_hueco**)a;
    t_hueco* huecoB = *(t_hueco**)b;

    if (huecoA->tamanio < huecoB->tamanio) {
        return -1;
    } else if (huecoA->tamanio > huecoB->tamanio) {
        return 1;
    } else {
        return 0;
    }
}


int compararHuecosPorTamanioDescendente(const void* a, const void* b) {
    t_hueco* huecoA = *(t_hueco**)a;
    t_hueco* huecoB = *(t_hueco**)b;

    if (huecoA->tamanio > huecoB->tamanio) {
        return -1;
    } else if (huecoA->tamanio < huecoB->tamanio) {
        return 1;
    } else {
        return 0;
    }
}