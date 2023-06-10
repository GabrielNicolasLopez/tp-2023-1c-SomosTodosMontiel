#include "kernel.h"

void implementar_fifo(){
	t_pcb *pcb = algoritmo_fifo(LISTA_READY); //Obtiene el 1er elemento de la lista de ready
	//log_info(logger, "Agregando UN pcb a lista exec");
	pasar_a_exec(pcb);
	// Cambio de estado
	//log_info(logger, "Cambio de Estado: PID %d - Estado Anterior: READY , Estado Actual: EXEC", pcb->pid);
	sem_post(&pasar_pcb_a_CPU);
}

t_pcb *algoritmo_fifo(t_list *LISTA_READY){
	t_pcb *pcb = (t_pcb *)list_remove(LISTA_READY, 0);
	return pcb;
}