#include "kernel.h"

void implementar_fifo(){
	t_pcb *pcb = algoritmo_fifo(LISTA_READY); //Obtiene el 1er elemento de la lista de ready
	//Mando la PCB a la cola de ejecucion
	pasar_a_exec(pcb);
	log_info(logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);
	//Aviso al hilo de KERNEL-CPU que puede mandar el contexto a CPU
	sem_post(&pasar_pcb_a_CPU);
}

t_pcb *algoritmo_fifo(t_list *LISTA_READY){
	t_pcb *pcb = (t_pcb *)list_remove(LISTA_READY, 0);
	return pcb;
}