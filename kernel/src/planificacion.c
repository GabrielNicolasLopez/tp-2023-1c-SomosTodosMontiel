#include "kernel.h"

void planiLargoPlazo(){
 	while (1){
 		//Espera a que haya alguna PCB disponible para pasarla a READY
 		sem_wait(&CantPCBNew);
 		agregar_pcb_a_ready();
 	}
}

void agregar_pcb_a_ready(){
	//Si la multiprogramacion me permite...
	sem_wait(&multiprogramacion);
	pthread_mutex_lock(&listaNew);
	//Saco el 1er elemento de la lista de new (ESO NO TIENE NADA QUE VER CON FIFO. SIEMPRE SE USA FIFO PARA SACAR PCBS DE NEW)
	t_pcb* pcb = list_remove(LISTA_NEW, 0); //Elimino la pcb de la lista
	pthread_mutex_unlock(&listaNew); 

	log_debug(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->contexto->pid);
	pasar_a_ready(pcb);
}

void planiCortoPlazo(){
	while (1){
		//Si hay alguna PCB en ready
		sem_wait(&cantPCBReady);
		//Y la CPU está vacía
		sem_wait(&CPUVacia);
		t_tipo_algoritmo algoritmo;
		//Dado el algoritmo, envio la pcb a ejecutar
		algoritmo = obtenerAlgoritmo();
		switch (algoritmo){
			case FIFO:
				log_info(logger, "Implementando algoritmo FIFO");
				implementar_fifo();
				break;
				
			case HRRN:
				log_info(logger, "Implementando algoritmo HRRN");
				implementar_hrrn();
				break;

			default:
				log_error(logger, "ERROR AL ELEGIR EL ALGORITMO EN EL PLANIFICADOR DE CORTO PLAZO");
				break;
		}
	}
}

t_tipo_algoritmo obtenerAlgoritmo(){
	char *algoritmoConfig = configuracionKernel->ALGORITMO_PLANIFICACION;
	t_tipo_algoritmo algoritmo;

	if(!strcmp(algoritmoConfig, "FIFO"))
		algoritmo = FIFO;
	else if(!strcmp(algoritmoConfig, "HRRN"))
		algoritmo = HRRN;
	else
		log_error(logger, "ALGORITMO ESCRITO INCORRECTAMENTE");
	return algoritmo;
}