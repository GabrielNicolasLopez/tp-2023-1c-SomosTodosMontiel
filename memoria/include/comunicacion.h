#ifndef COMUNICACION.H
#define COMUNICACION.H

#include <memoria.h>



//Definimos las funciones

void hilo_cpu();
void hilo_filesystem();
void hilo_kernel();


void recibirDatos(uint32_t pid ,uint32_t id, uint32_t tam);

void mandarLaBase(uint32_t baseMandar);

void recibirIDPID(uint32_t *id,uint32_t *pid);


#endif