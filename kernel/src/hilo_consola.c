#include "kernel.h"

void crear_hilo_consola(){
	int server_fd = iniciar_servidor("127.0.0.1", configuracionKernel->PUERTO_ESCUCHA);
	log_info(logger, "Kernel listo para recibir clientes consola");
	uint8_t handshake;
	while (1){
		pthread_t hilo_atender_consola;
		int socketCliente = esperar_cliente(server_fd);
		handshake = stream_recv_header(socketCliente);
		stream_recv_empty_buffer(socketCliente);
		if (handshake == HANDSHAKE_consola){
			log_info(logger, "Se envia handshake ok continue a consola");
			stream_send_empty_buffer(socketCliente, HANDSHAKE_ok_continue);

			log_debug(logger, "KERNEL SE CONECTO CON UNA CONSOLA");			
			
			t_datosCrearPCB *datos = malloc(sizeof(t_datosCrearPCB));
			t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
			datos->instrucciones = instrucciones;
			datos->instrucciones = recibir_instruciones_desde_consola(socketCliente);
			datos->socket = socketCliente;
			pthread_create(&hilo_atender_consola, NULL, (void *)crear_pcb, (void*)datos);
			pthread_detach(hilo_atender_consola);

		} else
			stream_send_empty_buffer(socketCliente, HANDSHAKE_error);
	}
}

t_instrucciones *recibir_instruciones_desde_consola(int cliente_fd){

	log_info(logger, "Recibiendo instrucciones desde consola...");
	
	int cantidad_de_instrucciones = 0;
	t_tipoInstruccion instruccion;
	t_instruccion* instruccionRecibida;
	t_instrucciones* instruccionesRecibidas = malloc(sizeof(t_instrucciones));
	instruccionesRecibidas->listaInstrucciones = list_create();

	t_Kernel_Consola header = stream_recv_header(cliente_fd);

	if(header != INSTRUCCIONES)
		log_error(logger, "El header que mando consola a kernel no es INSTRUCCIONES");	
	
    t_buffer* bufferInstrucciones = buffer_create();

    stream_recv_buffer(cliente_fd, bufferInstrucciones);
	log_error(logger, "Tamaño de las instrucciones recibidas %d", bufferInstrucciones->size);

	//Empiezo a desempaquetar
	buffer_unpack(bufferInstrucciones, &instruccion, sizeof(t_tipoInstruccion));

	while(instruccion != EXIT){
		instruccionRecibida = malloc(sizeof(t_instruccion));

		instruccionRecibida->tipo = instruccion;
		//log_debug(logger, "tipo de instruccion: %d", instruccion);
	
		if(instruccion == DELETE_SEGMENT)
			//Numero de segmento
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		if(instruccion == CREATE_SEGMENT){
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == SIGNAL){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == WAIT){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			//log_error(logger, "long wait: %d", instruccionRecibida->longitud_cadena);
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//log_error(logger, "cadena wait: %s", instruccionRecibida->cadena);
		}
		if(instruccion == F_TRUNCATE){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_WRITE){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_READ){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_SEEK){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_CLOSE){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == F_OPEN){
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == IO){
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == SET){
			//Registro
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->registro, sizeof(t_registro));
			//Longitud cadena
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(bufferInstrucciones, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//log_error(logger, "cadena set: %s", instruccionRecibida->cadena);
		}
		if(instruccion == MOV_IN){
			//Registro
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->registro, sizeof(t_registro));
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == MOV_OUT){
			//Parametro A
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Registro
			buffer_unpack(bufferInstrucciones, &instruccionRecibida->registro, sizeof(t_registro));
		}

		//Agregamos la instruccion recibida a la lista de instrucciones
		list_add(instruccionesRecibidas->listaInstrucciones, instruccionRecibida);
		cantidad_de_instrucciones++;

		//Obtengo una nueva instruccion
		buffer_unpack(bufferInstrucciones, &instruccion, sizeof(t_tipoInstruccion));
	}
	instruccionRecibida = malloc(sizeof(t_instruccion));
	//Instruccion EXIT
	instruccionRecibida->tipo = instruccion;
	//log_debug(logger, "tipo de instruccion: %d", instruccion);
	list_add(instruccionesRecibidas->listaInstrucciones, instruccionRecibida);
	cantidad_de_instrucciones++;
	//Asignamos la cantidad de instrucciones
	instruccionesRecibidas->cantidadInstrucciones = cantidad_de_instrucciones;

    buffer_destroy(bufferInstrucciones);

    return instruccionesRecibidas;
}

void crear_pcb(void *datos){
	t_datosCrearPCB *datosPCB = (t_datosCrearPCB *)datos;
	
	t_pcb *pcb 					           = malloc(sizeof(t_pcb));
	pcb->contexto                          = malloc(sizeof(t_contextoEjecucion));
	pcb->contexto->instrucciones           = malloc(sizeof(t_instrucciones));
	pcb->contexto->registrosCPU            = malloc(sizeof(t_registrosCPU));
	pcb->contexto->registrosCPU->registroC = malloc(sizeof(t_registroC));
	pcb->contexto->registrosCPU->registroE = malloc(sizeof(t_registroE));
	pcb->contexto->registrosCPU->registroR = malloc(sizeof(t_registroR));

	//Primero relleno los datos del contexto de ejecucion
	//PID
	pthread_mutex_lock(&PID);
	pcb->contexto->pid              = ++PID_PCB;
	pthread_mutex_unlock(&PID);
	//Socket
	pcb->contexto->socket           = datosPCB->socket;
	//PC
	pcb->contexto->program_counter  = 0;

	//Instrucciones
	pcb->contexto->instrucciones->cantidadInstrucciones = datosPCB->instrucciones->cantidadInstrucciones;
	pcb->contexto->instrucciones->listaInstrucciones    = list_create();
	pcb->contexto->instrucciones->listaInstrucciones    = list_duplicate(datosPCB->instrucciones->listaInstrucciones);
	
	//Inicializar registros en \0
	inicializar_registro_cpu(pcb);
	
	//Estimacion anterior
	pcb->estimacion_anterior = configuracionKernel->ESTIMACION_INICIAL;
	//Real anterior
	pcb->real_anterior       = 0;

	//Tabla de segmentos del proceso
	crear_tabla_de_segmentos(pcb);

	//Inicializo el semaforo de TAAP
	pthread_mutex_init(&pcb->mutex_TAAP, NULL);
	
	//Le pido a memoria el segmento 0
	//pedir_a_memoria_el_segmento0(pcb->contexto->pid);
	
	//t_segmento *segmento0 = malloc(sizeof(t_segmento));

	//segmento0->id_segmento =  0;
	//segmento0->base = 0;
	//Recibo la respuesta
	//segmento0->tamanio = recibir_el_segmento0_de_memoria();

	//Agrego el segmento 0 al proceso
	//agregar_segmento_0_a_pcb(pcb, segmento0); 

	//Tabla de archivos abiertos del proceso
	crear_tabla_de_archivos_proceso(pcb);

	//Una vez inicializada la PCB, la pasamos a NEW
	pasar_a_new(pcb);

	//Sumamos uno a la cantidad de PCB's en new
	sem_post(&CantPCBNew);
}

void pedir_a_memoria_el_segmento0(int pid){
	t_buffer *pedir_segmento0 = buffer_create();

	//Empaqueto el PID
	buffer_pack(pedir_segmento0, &pid, sizeof(uint32_t));

	stream_send_buffer(conexion_con_memoria, NUEVO_PROCESO, pedir_segmento0);

	buffer_destroy(pedir_segmento0);
}

uint32_t recibir_el_segmento0_de_memoria(){
	t_buffer *recibir_segmento0 = buffer_create();
	uint32_t tamanio;

	t_Kernel_Memoria header = stream_recv_header(conexion_con_memoria);

	if(header != NUEVO_PROCESO){
		log_error(logger, "no se recibió el header correcto al crear un proceso");
	}

	stream_recv_buffer(conexion_con_memoria, recibir_segmento0);

	//Desempaqueto la tamanio
	buffer_unpack(recibir_segmento0, &tamanio, sizeof(uint32_t));

	buffer_destroy(recibir_segmento0);

	return tamanio;
}

void crear_tabla_de_archivos_proceso(t_pcb *pcb){
	pcb->taap = list_create();
}

void agregar_segmento_0_a_pcb(t_pcb *pcb, t_segmento *segmento0){
	agregar_segmento(pcb, segmento0);
}

void agregar_segmento(t_pcb *pcb, t_segmento *segmento_a_agregar){
	list_add(pcb->tablaDeSegmentos, segmento_a_agregar);
}

void crear_tabla_de_segmentos(t_pcb *pcb){
	pcb->tablaDeSegmentos = list_create();
}

void inicializar_registro_cpu(t_pcb *pcb){
	char* valorInicial = "\0\0\0\0";
	//Resgitros C
	strcpy(pcb->contexto->registrosCPU->registroC->ax, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroC->bx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroC->cx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroC->dx, valorInicial);
	//Resgitros E
	strcpy(pcb->contexto->registrosCPU->registroE->eax, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroE->ebx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroE->ecx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroE->edx, valorInicial);
	//Resgitros R
	strcpy(pcb->contexto->registrosCPU->registroR->rax, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroR->rbx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroR->rcx, valorInicial);
	strcpy(pcb->contexto->registrosCPU->registroR->rdx, valorInicial);
}