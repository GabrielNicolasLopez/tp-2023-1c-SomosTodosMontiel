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

void asignarRecurso(char *nombre_recurso, t_pcb* pcb)
{
	for (int i = 0; i < list_size(lista_de_recursos); i++)
	{
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if (strcmp(recurso->nombre, nombre_recurso) == 0)
		{
			pthread_mutex_lock(&recurso->mutex_lista_blocked);
			recurso->instancias_recursos -= 1;
			pthread_mutex_unlock(&recurso->mutex_lista_blocked);
			//Agrego el recurso a la tabla de recursos
			pthread_mutex_lock(&pcb->mutex_TablaDeRecursos);
			list_add(pcb->tablaDeRecursos, recurso);
			pthread_mutex_unlock(&pcb->mutex_TablaDeRecursos);
		}
	}
	log_debug(logger, "PID: <%d> - Wait: <%s> - Instancias: <%d>", pcb->contexto->pid, nombre_recurso, recursos_disponibles(nombre_recurso));
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

void devolverRecursosPCB(t_pcb* pcb)
{
	if(list_size(pcb->tablaDeRecursos) > 0)
	{
		t_recurso *recursoADevolver = malloc(sizeof(t_recurso));
		while(!list_is_empty(pcb->tablaDeRecursos)){
			recursoADevolver = list_get(pcb->tablaDeRecursos, 0);
			devolverRecurso(recursoADevolver->nombre, pcb);
			log_error(logger, "recursos de pcb EN EXIT: %d", list_size(pcb->tablaDeRecursos));
		}
	}
}

void devolverRecurso(char *nombre_recurso, t_pcb* pcb) 
{
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++)
	{
		if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
	}

	t_recurso *recurso = list_get(lista_de_recursos, posicion);
	//log_error(logger, "se devuelve una instancia del recurso <%s>", recurso->nombre);
	pthread_mutex_lock(&recurso->mutex_lista_blocked);
	recurso->instancias_recursos += 1;
	pthread_mutex_unlock(&recurso->mutex_lista_blocked);

	log_error(logger, "recursos de pcb antes de eliminar: %d", list_size(pcb->tablaDeRecursos));
	//Remuevo el recurso de la tabla de recursos 
	list_remove_element(pcb->tablaDeRecursos, recurso);
	log_error(logger, "recursos de pcb despues de eliminar: %d", list_size(pcb->tablaDeRecursos));
	log_debug(logger, "PID: <%d> - Signal: <%s> - Instancias: <%d>", pcb->contexto->pid, nombre_recurso, recursos_disponibles(nombre_recurso));
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
		//Lo desbloqueamos
		t_pcb* pcb_blocked_a_ready = list_remove(recurso->lista_block, 0);
		//Asignamos el recurso
		asignarRecurso(nombre_recurso, pcb_blocked_a_ready);
		//Y lo mandamos a la cola de ready
		log_debug(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb_blocked_a_ready->contexto->pid);
		pasar_a_ready(pcb_blocked_a_ready);
	}
}