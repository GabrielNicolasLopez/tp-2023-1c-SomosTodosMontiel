#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "memoria.h"

t_segmento* segmentoCrear(int pid,int id, int base, int tam);
t_hueco* huecoCrear(t_segmento* segmento);
uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio);
uint32_t aplicarAlgoritmo(uint32_t tamSegmento);
t_tipo_algoritmo obtenerAlgoritmo();
t_segmento* buscarSegmentoPorIdPID(uint32_t id,uint32_t pid);
uint32_t compararPorBase(const void* a, const void* b);
t_list* buscarSegmentoPorPID(uint32_t pid);
void eliminarProceso(t_list* listaSegmentosBorrar);
void compactar();

#endif