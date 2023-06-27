#ifndef ALGORITMOS.H
#define ALGORITMOS.H

#include <memoria.h>



uint32_t asignarSegmentoFirstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);

void asignarSegmentoBestFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);


void asignarSegmentoWorstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);


int compararHuecosPorTamanioAscendente(const void* a, const void* b);

int compararHuecosPorTamanioDescendente(const void* a, const void* b);


#endif