#ifndef KERNEL_CPU_H
#define KERNEL_CPU_H

#include <stdio.h>
#include <commons/collections/list.h>
#include <shared_utils.h>

void instruccion_destroy(t_instruccion* );
void contexto_de_ejecucion_destroy(t_contextoEjecucion* );
int calcularSizeListaInstrucciones(t_instrucciones *);
//t_motivoDevolucion* recibir_contexto_y_motivo(int);

#endif