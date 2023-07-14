#include "kernel.h"

void pasar_a_new(t_pcb *pcb){
	pthread_mutex_lock(&listaNew);
	list_add(LISTA_NEW, pcb);
	pthread_mutex_unlock(&listaNew);
	log_debug(logger, "Se crea el proceso <%d> en NEW", pcb->contexto->pid);
	
}

void pasar_a_ready(t_pcb *pcb){
	pthread_mutex_lock(&listaReady);
	list_add(LISTA_READY, pcb);
	struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
	pcb->llegadaReady = start;
	pthread_mutex_unlock(&listaReady);
	log_debug(logger, "Cola Ready <%s>: ", configuracionKernel->ALGORITMO_PLANIFICACION);
	for(int i=0; i<list_size(LISTA_READY); i++){
		t_pcb* pcb = list_get(LISTA_READY, i);
		log_debug(logger, "%d", pcb->contexto->pid);
	}
	sem_post(&cantPCBReady);
}

void pasar_a_exec(t_pcb *pcb){
	pthread_mutex_lock(&listaExec);
	list_add(LISTA_EXEC, pcb);
	pthread_mutex_unlock(&listaExec);
}

void pasar_a_blocked(t_pcb* pcb){
	pthread_mutex_lock(&listaBlocked);
	list_add(LISTA_BLOCKED, pcb);
	pthread_mutex_unlock(&listaBlocked);
	//log_debug(logger, "Paso a BLOCKED el proceso %d", pcb->contexto->pid);
}

void pasar_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&listaExit);
	list_add(LISTA_EXIT, pcb);
	pthread_mutex_unlock(&listaExit);
	//log_debug(logger, "Paso a EXIT el proceso %d", pcb->contexto->pid);
}


