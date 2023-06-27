#include <algoritmos.h>


//FirstFit
uint32_t algoritmoFirstFit(uint32_t tamSegmento) {

    // Buscar el primer hueco que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        uint32_t baseEncontrada;
        if (hueco->tamanio >= tamSegmento) {
            baseEncontrada = hueco->base;

            // Actualizar el hueco libre
            hueco->base = hueco->base+tamSegmento;
            hueco->tamanio =hueco->tamanio -tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return baseEncontrada;
        }
    }

     
}

//BestFit
uint32_t algoritmoBestFit(uint32_t tamSegmento) {
    // Ordenar los huecos por tamaño de forma ascendente
    list_sort(listaHuecos, compararHuecosPorTamanioAscendente);

    // Buscar el hueco más pequeño que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        uint32_t baseEncontrada;
        if (hueco->tamanio >= tamSegmento) {
            baseEncontrada = hueco->base;

            // Actualizar el hueco libre
            hueco->base += tamSegmento;
            hueco->tamanio -= tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return baseEncontrada;
        }
    }

    
}

//Worts Fit
uint32_t algoritmoWorstFit(uint32_t tamSegmento) {
    // Ordenar los huecos por tamaño de forma descendente
    list_sort(listaHuecos, compararHuecosPorTamanioDescendente);

    // Buscar el hueco más grande que pueda contener el segmento
    for (int i = 0; i < list_size(listaHuecos); i++) {
        t_hueco* hueco = list_get(listaHuecos, i);
        uint32_t baseEncontrada;
        if (hueco->tamanio >= tamSegmento) {
            baseEncontrada = hueco->base;

            // Actualizar el hueco libre
            hueco->base += tamSegmento;
            hueco->tamanio -= tamSegmento;

            // Eliminar el hueco si queda vacío
            if (hueco->tamanio == 0) {
                free(list_remove(listaHuecos, i));
            }

            return baseEncontrada;
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