#include "kernel.h"

void pasar_a_new(t_pcb *pcb){
	pthread_mutex_lock(&listaNew);
	list_add(LISTA_NEW, pcb);
	pthread_mutex_unlock(&listaNew);
	log_debug(logger, "Paso a NEW el proceso %d", pcb->contexto->pid);
}

void pasar_a_ready(t_pcb *pcb){
	pthread_mutex_lock(&listaReady);
	list_add(LISTA_READY, pcb);
	struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
	pcb->llegadaReady = start;
	pthread_mutex_unlock(&listaReady);
	sem_post(&cantPCBReady);
	log_debug(logger, "Paso a READY el proceso %d", pcb->contexto->pid);
}

void pasar_a_exec(t_pcb *pcb){
	pthread_mutex_lock(&listaExec);
	list_add(LISTA_EXEC, pcb);
	pthread_mutex_unlock(&listaExec);
	log_debug(logger, "Paso a EXEC el proceso %d", pcb->contexto->pid);
}

void pasar_a_blocked(t_pcb* pcb){
	pthread_mutex_lock(&listaBlocked);
	list_add(LISTA_BLOCKED, pcb);
	pthread_mutex_unlock(&listaBlocked);
	log_debug(logger, "Paso a BLOCKED el proceso %d", pcb->contexto->pid);
}

void pasar_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&listaExit);
	list_add(LISTA_EXIT, pcb);
	pthread_mutex_unlock(&listaExit);
	log_debug(logger, "Paso a EXIT el proceso %d", pcb->contexto->pid);
}


