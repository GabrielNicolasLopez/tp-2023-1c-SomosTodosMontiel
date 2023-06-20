#include "kernel.h"

int recursos_disponibles(char *nombre_recurso){
	int recursos_disponibles;
	for (int i = 0; i < list_size(lista_de_recursos); i++)
	{
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if (strcmp(recurso->nombre, nombre_recurso) == 0)
			recursos_disponibles = recurso->instancias_recursos;
	}
	return recursos_disponibles;
}

bool existeRecurso(char *nombre_recurso)
{
	for (int i = 0; i < list_size(lista_de_recursos); i++)
	{
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if (strcmp(recurso->nombre, nombre_recurso) == 0)
			return true;
	}
	return false;
}

void asignarRecurso(char *nombre_recurso)
{
	for (int i = 0; i < list_size(lista_de_recursos); i++)
	{
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if (strcmp(recurso->nombre, nombre_recurso) == 0)
		{
			pthread_mutex_lock(&recurso->mutex_lista_blocked);
			recurso->instancias_recursos -= 1;
			pthread_mutex_unlock(&recurso->mutex_lista_blocked);
		}
	}
}

void pasar_a_blocked_de_recurso(t_pcb *pcb_a_blocked, char *nombre_recurso)
{
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++)
	{
		if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
	}

	t_recurso *recurso = list_get(lista_de_recursos, posicion);
	log_debug(logger, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", pcb_a_blocked->contexto->pid);
	log_debug(logger, "PID: <%d> - Bloqueado por: <%s>", pcb_a_blocked->contexto->pid, nombre_recurso);
	pthread_mutex_lock(&recurso->mutex_lista_blocked);
	list_add(recurso->lista_block, pcb_a_blocked);
	pthread_mutex_unlock(&recurso->mutex_lista_blocked);
}

void devolverRecurso(char *nombre_recurso)
{
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++)
	{
		if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
	}

	t_recurso *recurso = list_get(lista_de_recursos, posicion);

	pthread_mutex_lock(&recurso->mutex_lista_blocked);
	recurso->instancias_recursos += 1;
	pthread_mutex_unlock(&recurso->mutex_lista_blocked);
}

void actualizar_procesos_bloqueados(char *nombre_recurso){

	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++)
	{
		if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
	}

	t_recurso *recurso = list_get(lista_de_recursos, posicion);

	if(list_size(recurso->lista_block) > 0){ 
		//Si existe algun proceso bloqueado esperando un recurso que se acaba de devolver
		//Asignamos el recurso
		asignarRecurso(nombre_recurso);
		//Lo desbloqueamos
		t_pcb* pcb_blocked_a_ready = list_remove(recurso->lista_block, 0);
		//Y lo mandamos a la cola de ready
		log_debug(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb_blocked_a_ready->contexto->pid);
		pasar_a_ready(pcb_blocked_a_ready);
	}
}