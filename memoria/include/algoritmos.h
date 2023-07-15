#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "memoria.h"


//Algoritmos
uint32_t algoritmoFirstFit(uint32_t tamSegmento); 

uint32_t algoritmoBestFit(uint32_t tamSegmento);

uint32_t algoritmoWorstFit(uint32_t tamSegmento);

bool compararHuecosPorTamanioAscendente(const void* a, const void* b);

bool compararHuecosPorTamanioDescendente(const void* a, const void* b);

#endif