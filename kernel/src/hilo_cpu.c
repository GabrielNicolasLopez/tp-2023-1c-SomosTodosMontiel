#include "kernel.h"

void crear_hilo_cpu(){
	// Me conecto a cpu
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	if(conexion_con_cpu == -1){
		log_error(logger, "KERNEL NO SE CONECTÓ CON CPU. FINALIZANDO KERNEL...");
		//exit(1);
	}

	log_info(logger, "KERNEL SE CONECTO CON CPU...");
	//En esta variable se guarda el estado del semaforo de multiprogramacion
	//v=valor mp = multiprogramacion
	//int v_mp;

	t_motivoDevolucion* motivoDevolucion = malloc(sizeof(t_motivoDevolucion));
	t_contextoEjecucion* contextoEjecucion = malloc(sizeof(t_contextoEjecucion));
	motivoDevolucion->contextoEjecucion = contextoEjecucion;

	while (1){
		if(!se_reenvia_el_contexto){
			log_debug(logger, "Esperando la llegada de una nueva PCB.");
			//Espero a que un proceso este en la cola de ejecucion
			sem_wait(&pasar_pcb_a_CPU);

			//Obtengo la pcb en ejecucion para crear el contexto de ejecucion
			t_pcb* pcb = pcb_ejecutando();
			
			//Enviamos el contexto de ejecucion a cpu
			log_debug(logger, "ce enviado a CPU. PID: %d", pcb->contexto->pid);
			enviar_ce_a_cpu(pcb->contexto, conexion_con_cpu);
		}

		//char *mensaje = recibirMensaje(conexion_con_cpu);
		//log_info(logger, "CPU dice: %s", mensaje);			
		
		log_debug(logger, "cym recibido desde CPU.");
		recibir_cym_desde_cpu(motivoDevolucion, conexion_con_cpu);

		switch (motivoDevolucion -> tipo){
			case EXIT:
				se_reenvia_el_contexto = false;
				terminar_consola(motivoDevolucion);
				break;
			case WAIT:
				//Si no existe el recurso
				if(!existeRecurso(motivoDevolucion -> cadena)){
					log_info(logger, "PID %d ha sido finalizado por el dibu porque no existe el recurso: %s.\n", motivoDevolucion -> contextoEjecucion -> pid, motivoDevolucion -> cadena);
					se_reenvia_el_contexto = false;
					terminar_consola(motivoDevolucion); //Explicar el error por el cual se termina
					break;
				}
				//Si existe el recurso (el nombre de recurso viene dentro de cadena)
				//Si hay recursos disponibles (hay mas de 0)
				if(recursos_disponibles(motivoDevolucion->cadena) > 0){
					//Le asignamos a la PCB ejecutando el recurso pedido
					asignarRecurso(motivoDevolucion -> cadena);
					log_info(logger, "PID %d robó un recurso: %s", motivoDevolucion -> contextoEjecucion-> pid, motivoDevolucion -> cadena);
					se_reenvia_el_contexto = true;
					devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);	
					break;
				}
				//Si el recurso existe pero no hay instancias disponibles en este momento, se envia a la lista de bloqueo
				else if(recursos_disponibles(motivoDevolucion -> cadena) <= 0){
					//Le pasamos la pcb y el nombre del recurso para que lo bloquee 
					pasar_a_blocked_de_recurso(pcb_ejecutando(), motivoDevolucion -> cadena);
					se_reenvia_el_contexto = false;
					sem_post(&CPUVacia);
				}
				break;
			case SIGNAL:
				//Si no existe el recurso
				if(!existeRecurso(motivoDevolucion -> cadena)){
					log_info(logger, "PID %d ha sido finalizado por el dibu porque no existe el recurso: %s.\n", motivoDevolucion -> contextoEjecucion -> pid, motivoDevolucion -> cadena);
					se_reenvia_el_contexto = false;
					terminar_consola(motivoDevolucion); //Explicar el error por el cual se termina
					sem_post(&CPUVacia);
					break;
				}
				//Si existe el recurso
				devolverRecurso(motivoDevolucion -> cadena);
				log_info(logger, "PID %d devolvio una instancia del recurso: %s", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cadena);
				se_reenvia_el_contexto = true;
				devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);	
				break;

			case YIELD: 
				pasar_a_ready(pcb_ejecutando());
				break;
				
			case IO: 
				pthread_t hilo_sleep;
				pasar_a_blocked(pcb_ejecutando_remove());
				sem_post(&CPUVacia);

				pthread_create(&hilo_sleep, NULL,(void *)sleep_IO, (void*) motivoDevolucion);
				//Detacheamos el hilo que es quien se encarga de mandar a ready al pcb
				pthread_detach(hilo_sleep);

				break;
			default:
				log_error(logger, "ERROR EN LA RESPUESTA DE CPU");
				break;
		}

		// Revisar/Sacar
		/*
		sem_post(&CPUVacia);
		sem_post(&multiprogramacion);
		sem_getvalue(&multiprogramacion, &v_mp);
		log_debug(logger, "MP=%d", v_mp);*/
	}
}

void devolver_ce_a_cpu(t_contextoEjecucion *contextoEjecucion, int conexion_con_cpu){
	enviar_ce_a_cpu(contextoEjecucion, conexion_con_cpu);
}

void recibir_cym_desde_cpu(t_motivoDevolucion *motivoDevolucion, int conexion_con_cpu){
	
	t_buffer* cym_recibido = buffer_create();

	t_contextoEjecucion* contextoEjecucion = malloc(sizeof(t_contextoEjecucion));
	t_instrucciones *inst = malloc(sizeof(t_instrucciones));
	
	/*t_registrosCPU *registros = malloc(sizeof(t_registrosCPU));
	t_registroC *registroC    = malloc(sizeof(t_registroC));
	t_registroE *registroE    = malloc(sizeof(t_registroE));
	t_registroR *registroR    = malloc(sizeof(t_registroR));*/
	
	contextoEjecucion->instrucciones = inst;
	contextoEjecucion->instrucciones->listaInstrucciones = list_create();
	/*contextoEjecucion->registrosCPU = registros;
	contextoEjecucion->registrosCPU->registroC = registroC;
	contextoEjecucion->registrosCPU->registroE = registroE;
	contextoEjecucion->registrosCPU->registroR = registroR;*/

	motivoDevolucion->contextoEjecucion = contextoEjecucion;


    stream_recv_buffer(conexion_con_cpu, cym_recibido);
	log_error(logger, "Tamaño de cym recibido de cpu %d", cym_recibido->size);

	//Motivo = tipo de instruccion
    buffer_unpack(cym_recibido, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);
	
	//Cantidad entero = numero entero
	buffer_unpack(cym_recibido, &motivoDevolucion->cant_int, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);
	
	//Longitud de la cadena
	buffer_unpack(cym_recibido, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
	//log_debug(logger, "long: %d", motivoDevolucion->longitud_cadena);
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);
	
	//Cadena
	motivoDevolucion->cadena = malloc(motivoDevolucion->longitud_cadena);
	buffer_unpack(cym_recibido, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	//Socket
	buffer_unpack(cym_recibido, &contextoEjecucion->socket, sizeof(uint32_t));
	//log_debug(logger, "socket: %d", contextoEjecucion->socket);
	
	//PID
	buffer_unpack(cym_recibido, &contextoEjecucion->pid, sizeof(uint32_t));
	//log_debug(logger, "pid: %d", contextoEjecucion->pid);
	
	//PC
	buffer_unpack(cym_recibido, &contextoEjecucion->program_counter, sizeof(uint32_t));
	//log_debug(logger, "program_counter: %d", contextoEjecucion->program_counter);
	
	//Tamaño de tabla
	buffer_unpack(cym_recibido, &contextoEjecucion->tamanio_tabla, sizeof(uint32_t));
	//log_debug(logger, "tamanio_tabla: %d", contextoEjecucion->tamanio_tabla);

	
	t_tipoInstruccion instruccion;
	t_instruccion* instruccionRecibida;
	int cantidad_de_instrucciones = 0;
	//Empiezo a desempaquetar las instrucciones
	buffer_unpack(cym_recibido, &instruccion, sizeof(t_tipoInstruccion));

	while(instruccion != EXIT){
		instruccionRecibida = malloc(sizeof(t_instruccion));

		instruccionRecibida->tipo = instruccion;
		//log_debug(logger, "tipo de instruccion: %d", instruccion);
	
		if(instruccion == DELETE_SEGMENT)
			//Numero de segmento
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		if(instruccion == CREATE_SEGMENT){
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == SIGNAL){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == WAIT){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			//log_error(logger, "long: %d", instruccionRecibida->longitud_cadena);
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == F_TRUNCATE){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_WRITE){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_READ){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_SEEK){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_CLOSE){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == F_OPEN){
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == IO){
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == SET){
			//Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			//Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == MOV_IN){
			//Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == MOV_OUT){
			//Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
		}

		//Agregamos la instruccion recibida a la lista de instrucciones
		list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
		cantidad_de_instrucciones++;

		//Obtengo una nueva instruccion
		buffer_unpack(cym_recibido, &instruccion, sizeof(t_tipoInstruccion));
	}
	instruccionRecibida = malloc(sizeof(t_instruccion));
	//Instruccion EXIT
	instruccionRecibida->tipo = instruccion;
	//log_debug(logger, "tipo de instruccion: %d", instruccion);
	list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
	cantidad_de_instrucciones++;
	//Asignamos la cantidad de instrucciones
	contextoEjecucion->instrucciones->cantidadInstrucciones = cantidad_de_instrucciones;

    buffer_destroy(cym_recibido);

}

void enviar_ce_a_cpu(t_contextoEjecucion* contextoEjecucion, int conexion_con_cpu){

    t_buffer* ce_a_enviar = buffer_create();

	//Socket
	buffer_pack(ce_a_enviar, &contextoEjecucion->socket, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	//PID
	buffer_pack(ce_a_enviar, &contextoEjecucion->pid, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	//PC
	buffer_pack(ce_a_enviar, &contextoEjecucion->program_counter, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	//Tamaño de tabla
	buffer_pack(ce_a_enviar, &contextoEjecucion->tamanio_tabla, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

	//Instrucciones
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	for(int i=0; i<contextoEjecucion->instrucciones->cantidadInstrucciones; i++){
		instruccion = list_get(contextoEjecucion->instrucciones->listaInstrucciones, i);
		//log_debug(logger, "obtuve la instruccion: %d", i+1);
		buffer_pack(ce_a_enviar, &instruccion->tipo, sizeof(t_tipoInstruccion));
		//log_error(logger, "TAMAÑO DEL BUFFER I%d", ce_a_enviar->size);

		if (instruccion->tipo == SET){
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == MOV_IN){
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == MOV_OUT){
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == IO){
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_OPEN){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_CLOSE){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_SEEK){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_READ){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_WRITE){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == F_TRUNCATE){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == WAIT){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == SIGNAL){
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);		
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
				
		}
		else if (instruccion->tipo == CREATE_SEGMENT){
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == DELETE_SEGMENT){
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == YIELD){
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
		else if (instruccion->tipo == EXIT){
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

		}
	}

	//Tabla de segmentos
	//pack de tabla de segmentos coming soon

	stream_send_buffer(conexion_con_cpu, ce_a_enviar);
	log_error(logger, "Tamaño del CE enviado a CPU %d", ce_a_enviar->size);

    buffer_destroy(ce_a_enviar);
}

void update_program_counter(t_pcb *pcb, t_motivoDevolucion *motivoDevolucion){
	pcb->contexto->program_counter= motivoDevolucion->contextoEjecucion->program_counter;
};

void sleep_IO(t_motivoDevolucion *motivoDevolucion){
	int tiempo = motivoDevolucion->cant_int;
	sleep(tiempo);
	pasar_a_ready(pcb_ejecutando());
}

void terminar_consola(t_motivoDevolucion* motivoDevolucion){
	//Envio un paquete para que consola finalice
	t_paquete* paquete = crear_paquete();
	//Obtener el socket mediante pcb_ejecutando()
	paquete->codigo_operacion = TERMINAR_CONSOLA;
	agregar_a_paquete(paquete, NULL, 0);
	enviar_paquete(paquete, motivoDevolucion -> contextoEjecucion -> socket);
	log_error(logger, "FINALIZO EL PROCESO: %d", motivoDevolucion -> contextoEjecucion -> pid);
	t_pcb* pcb = pcb_ejecutando();
	pasar_a_exit(pcb);
	//Aumentamos el grado de multiprogramacion para que entre un nuevo proceso
	sem_post(&multiprogramacion);
}

t_pcb* pcb_ejecutando(){
	//t_pcb* pcb = malloc(sizeof(t_pcb));
	log_debug(logger, "Obteniendo pcb ejecutando");
	pthread_mutex_lock(&listaExec);
	t_pcb* pcb = list_get(LISTA_EXEC, 0);
	pthread_mutex_unlock(&listaExec);
	return pcb;
}

t_pcb *pcb_ejecutando_remove(){
	log_debug(logger, "Obteniendo y eliminando pcb ejecutando");
	pthread_mutex_lock(&listaExec);
	t_pcb* pcb = list_get(LISTA_EXEC, 0); //Primero obtengo una copia de la pcb
	list_remove(LISTA_EXEC, 0); //Elimino la pcb de la lista
	pthread_mutex_unlock(&listaExec);
	return pcb;
}

int recursos_disponibles(char* nombre_recurso){
	int recursos_disponibles;
	for (int i = 0; i < list_size(lista_de_recursos); i++){
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if(strcmp(recurso->nombre, nombre_recurso) == 0)
		recursos_disponibles = recurso->instancias_recursos;
	}
	return recursos_disponibles;
}

bool existeRecurso(char* nombre_recurso){
	for (int i = 0; i < list_size(lista_de_recursos); i++){
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if(strcmp(recurso->nombre, nombre_recurso) == 0)
			return true;
	}
	return false;
}

void asignarRecurso(char* nombre_recurso){
	for (int i = 0; i < list_size(lista_de_recursos); i++){
		t_recurso *recurso = list_get(lista_de_recursos, i);
		if(strcmp(recurso->nombre, nombre_recurso) == 0){
			pthread_mutex_lock(&recurso->mutex_lista_blocked);
			recurso->instancias_recursos -=1;
			pthread_mutex_unlock(&recurso->mutex_lista_blocked);
		}
	}
}

/*void asignarRecurso(char* nombre_recurso){
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){
        if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
    }
	//Resto un recurso
	t_recurso *recurso = list_get(lista_de_recursos, posicion);
	pthread_mutex_lock(recurso->mutex_lista_blocked);
 	recurso->instancias_recursos -=1;
  	pthread_mutex_unlock(recurso->mutex_lista_blocked);
}*/

void pasar_a_blocked_de_recurso(t_pcb* pcb, char* nombre_recurso){
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){
        if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
    }

	t_recurso *recurso = list_get(lista_de_recursos, posicion);
	pthread_mutex_lock(&recurso->mutex_lista_blocked);
	list_add(recurso->lista_block, pcb); 	
  	pthread_mutex_unlock(&recurso->mutex_lista_blocked);
	log_debug(logger, "Paso a blocked el proceso PID: %d del recurso %s", pcb->contexto->pid, recurso->nombre);
}

void devolverRecurso(char* nombre_recurso){
	int posicion;
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){
        if (strcmp(configuracionKernel->RECURSOS[i], nombre_recurso) == 0)
			posicion = i;
    }

	t_recurso *recurso = list_get(lista_de_recursos, posicion);

	pthread_mutex_lock(&recurso->mutex_lista_blocked);
	recurso->instancias_recursos+= 1;
	pthread_mutex_unlock(&recurso->mutex_lista_blocked);
}
