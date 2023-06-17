#include "kernel.h"

void planiLargoPlazo(){
 	while (1){
 		//Espera a que haya alguna PCB disponible para pasarla a READY
 		sem_wait(&CantPCBNew);
 		agregar_pcb();
 	}
}

void agregar_pcb(){
	//int v_mp;
	//sem_getvalue(&multiprogramacion, &v_mp);
	//log_debug(logger, "MP=%d", v_mp);
	sem_wait(&multiprogramacion);
	//sem_getvalue(&multiprogramacion, &v_mp);
	//log_debug(logger, "MP=%d", v_mp);
	//Bloqueo la lista de NEW para sacar una pcb
	pthread_mutex_lock(&listaNew);
	//Saco el 1er elemento de la lista de new (ESO NO TIENE NADA QUE VER CON FIFO. SIEMPRE SE USA FIFO PARA SACAR PCBS DE NEW)
	//t_pcb *pcb = list_remove(LISTA_NEW, 0); 
	t_pcb* pcb = list_get(LISTA_NEW, 0); //Primero obtengo una copia de la pcb
	list_remove(LISTA_NEW, 0); //Elimino la pcb de la lista
	//Desbloqueo la lista para que la puedan usar los otros hilos
	pthread_mutex_unlock(&listaNew); 

	log_debug(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->contexto->pid);
	pasar_a_ready(pcb);
}

void planiCortoPlazo(){
	//int cantPCB, CPUVaciaa;
	while (1){
		//sem_getvalue(&cantPCBReady, &cantPCB);
		//log_debug(logger, "cantPCBReady=%d", cantPCB);
		sem_wait(&cantPCBReady);
		//log_debug(logger, "pasamos el wait de cantready");
		//sem_getvalue(&CPUVacia, &CPUVaciaa);
		//log_debug(logger, "CPUVaciaa=%d", CPUVaciaa);
		sem_wait(&CPUVacia);
		//log_debug(logger, "pasamos el wait de cpuvacia");
		//log_info(logger, "Llego pcb a plani corto plazo");
		t_tipo_algoritmo algoritmo;
		algoritmo = obtenerAlgoritmo();

		switch (algoritmo){
		case FIFO:
			log_info(logger, "Implementando algoritmo FIFO");
			//log_debug(logger, " Cola Ready FIFO:");
			//cargarListaReadyIdPCB(LISTA_READY);
			implementar_fifo();

			break;
		case HRRN:
			log_info(logger, "Implementando algoritmo HRRN");
			//log_debug(logger, " Cola Ready RR:");
			//cargarListaReadyIdPCB(LISTA_READY);
			implementar_hrrn();

			break;
		default:
			log_info(logger, "ERROR AL ELEGIR EL ALGORITMO EN EL PLANIFICADOR DE CORTO PLAZO");
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