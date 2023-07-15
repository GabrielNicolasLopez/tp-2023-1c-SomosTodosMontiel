#include <algoritmos.h>
#include <funciones.h>


//FirstFit
uint32_t algoritmoFirstFit(uint32_t tamSegmento){
    log_debug(logger,"Aplicando Algoritmo FirstFit");
    list_sort(listaHuecos, compararPorBaseHueco);
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

   /*  log_debug(logger, "cantidad de segmentos en algoritmo bestfit: %d", list_size(listaHuecos)); */
    log_debug(logger,"Aplicando Algoritmo BestFit");
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
    log_debug(logger,"Aplicando Algoritmo WorstFit");
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

bool compararHuecosPorTamanioAscendente(const void* a, const void* b) {
    t_hueco* huecoA = (t_hueco*)a;
    t_hueco* huecoB = (t_hueco*)b;

    if (huecoA->tamanio < huecoB->tamanio)
        return true;
    return false;
}

bool compararHuecosPorTamanioDescendente(const void* a, const void* b) {
    t_hueco* huecoA = (t_hueco*)a;
    t_hueco* huecoB = (t_hueco*)b;

    if (huecoA->tamanio > huecoB->tamanio)
        return true;
    return false;
}