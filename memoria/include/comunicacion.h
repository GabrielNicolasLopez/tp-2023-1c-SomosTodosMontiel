#ifndef COMUNICACION_H
#define COMUNICACION_H

#include "memoria.h"


//Definimos las funciones
void hilo_cpu();
void hilo_filesystem();
void hilo_kernel_m();

//RECIBIR K
void recibirDatos(t_segmento *segmentoACrear);
void recibirIDPID(uint32_t *id,uint32_t *pid);
uint32_t recibirPID();

//MANDAR K
void mandarLaBase(uint32_t baseMandar);
void mandarTam();
void mandarPrBr();
void mandarListaProceso(t_list *lista);

//LEER CPU
void leer_CPU(char *dato, uint32_t dirF,uint32_t cantBytes, uint32_t pid);
void pedidoLectura_CPU(uint32_t *cantBytes, uint32_t *dir_fisica ,uint32_t *pid);
void enviarDato_CPU( uint32_t cantBytes, char* dato);

//ESCRIBIR CPU
void pedidoEscritura_CPU(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato,uint32_t *pid);
void escribir_CPU(char* dato, uint32_t dirF,uint32_t cantBytes,uint32_t pid );
void ok_CPU();

//LEER FS
void leer_FS(char* dato , uint32_t dirF,uint32_t cantBytes/* , uint32_t pid */);
void pedidoLectura_FS(uint32_t *cantBytes, uint32_t *dir_fisica/* ,uint32_t *pid */);
void enviarDato_FS(uint32_t cantBytes, char* dato);

//ESCRIBIR FS
void pedidoEscritura_FS(uint32_t *cantBytes, uint32_t *dir_fisica,char* dato/* ,uint32_t *pid */);
void escribir_FS(char* dato , uint32_t dirF,uint32_t cantBytes/* ,uint32_t pid */);
void ok_FS();


#endif