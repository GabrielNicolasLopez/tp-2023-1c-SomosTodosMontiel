#ifndef FUNCIONES.H
#define FUNCIONES.H

#include <memoria.h>


t_segmento* segmentoCrear(int id, int base, int tam);

uint32_t comprobar_Creacion_de_Seg(uint32_t tamanio);

void * eliminnar_segmento(t_segmento* segmento);

t_tipo_algoritmo obtenerAlgoritmo();




#endif