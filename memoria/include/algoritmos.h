#ifndef ALGORITMOS.H
#define ALGORITMOS.H

#include <memoria.h>


//Algoritmos
uint32_t algoritmoFirstFit(uint32_t tamSegmento); 

uint32_t algoritmoBestFit(uint32_t tamSegmento);

uint32_t algoritmoWorstFit(uint32_t tamSegmento);

int compararHuecosPorTamanioAscendente(const void* a, const void* b);

int compararHuecosPorTamanioDescendente(const void* a, const void* b);

#endif