#include "kernel_cpu.h"

// LIBERAR UNA INSTRUCCIONS
void instruccion_destroy(t_instruccion* inst)
{
	free(inst->cadena);
	free(inst);
} 

// LIBERAR CONTEXTO DE EJECUCION
void contexto_de_ejecucion_destroy(t_contextoEjecucion* contextoEjecucion)
{
	list_destroy_and_destroy_elements(contextoEjecucion->instrucciones->listaInstrucciones, instruccion_destroy);
	free(contextoEjecucion);
}

// CALCULAR TAMAÑOS PARA BUFFERS
int calcularSizeListaInstrucciones(t_instrucciones *instrucciones){
	int total = 0;
	for (int i = 0 ; i < list_size(instrucciones->listaInstrucciones); i++){
		
		t_instruccion* instruccion = list_get(instrucciones->listaInstrucciones,i);
		total += sizeof(t_tipoInstruccion);
		total += sizeof(t_registro) * 2;
		total += sizeof(uint32_t) * 2;
		total += sizeof(instruccion->cadena);
		//log_debug(logger, "total: %d", sizeof(total));
	}
	return total;
}

