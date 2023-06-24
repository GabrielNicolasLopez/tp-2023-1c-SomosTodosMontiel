#ifndef FUNCIONES.H
#define FUNCIONES.H

#include <memoria.h>

t_segmento* segmentoCrear(int id, int base, int tam);

uint32_t comprobar_Creacion_de_Seg(t_segmento** tablaSegmentos, t_hueco** tablaHuecos,uint32_t id, uint32_t tam);

void * eliminnar_segmento(t_segmento* segmento);

uint32_t asignarSegmentoFirstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);

void asignarSegmentoBestFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);


void asignarSegmentoWorstFit(t_list* listaSegmentos, t_list* listaHuecos, uint32_t idProceso, uint32_t tamSegmento);

int compararHuecosPorTamanioAscendente(const void* a, const void* b);

int compararHuecosPorTamanioDescendente(const void* a, const void* b);



#endif