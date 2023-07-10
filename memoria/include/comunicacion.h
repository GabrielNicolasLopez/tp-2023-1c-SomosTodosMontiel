#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "memoria.h"



//Definimos las funciones

void hilo_cpu();
void hilo_filesystem();
void hilo_kernel();


void recibirDatos(uint32_t pid ,uint32_t id, uint32_t tam);

void mandarLaBase(uint32_t baseMandar);

void recibirIDPID(uint32_t *id,uint32_t *pid);

void mandarListaGlobal();
void mandarListaProceso(t_list *lista);
void mandarPrBr();
void mandarTam();
void recibirPID(uint32_t *pid);

void leer_CPU(uint32_t* dato , uint32_t dirF,uint32_t pid);
void pedidoLectura_CPU(uint32_t *dir_fisica,uint32_t *pid);
void enviarDato_CPU(uint8_t dato);
void pedidoEscritura_CPU(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato,uint32_t *pid);
void escribir_CPU(uint32_t* dato , uint32_t dirF,uint32_t cantBytes,uint32_t pid);
void ok_CPU();

void leer_FS(uint32_t* dato , uint32_t dirF,uint32_t cantBytes,uint32_t pid);
void pedidoLectura_FS(uint32_t *cantBytes, uint32_t *dir_fisica,uint32_t *pid);
void enviarDato_FS(uint8_t dato);
void pedidoEscritura_FS(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato,uint32_t *pid);
void escribir_FS(uint32_t* dato , uint32_t dirF,uint32_t cantBytes,uint32_t pid);
void ok_FS();


#endif