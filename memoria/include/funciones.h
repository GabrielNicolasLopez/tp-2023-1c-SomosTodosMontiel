#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "memoria.h"


t_segmento* segmentoCrear(int pid,int id, int base, int tam);

uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio);

void * eliminnar_segmento(t_segmento* segmento);

t_tipo_algoritmo obtenerAlgoritmo();

void compactar();
t_hueco* huecoCrear(t_segmento* segmento);
t_segmento* buscarSegmentoPorIdPID(uint32_t id,uint32_t pid);
uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio);
void eliminarProceso(t_list* listaSegmentosBorrar);
uint32_t aplicarAlgoritmo(uint32_t tamSegmento);



#endif