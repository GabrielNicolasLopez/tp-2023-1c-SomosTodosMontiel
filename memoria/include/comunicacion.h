#ifndef COMUNICACION.H
#define COMUNICACION.H

#include <memoria.h>
#include <funciones.h>

//Definimos las funciones

t_segmento *recibirIDTam(uint32_t id, uint32_t tam);

void mandarLaBase(uint32_t baseMandar);

uint32_t recibirID(uint32_t id);


#endif