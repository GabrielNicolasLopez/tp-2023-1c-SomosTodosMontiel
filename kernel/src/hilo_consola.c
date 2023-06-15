#include "kernel.h"

void crear_hilo_consola(){
	int server_fd = iniciar_servidor("127.0.0.1", configuracionKernel->PUERTO_ESCUCHA, logger);
	log_info(logger, "Kernel listo para recibir clientes consola");

	while (1){
		pthread_t hilo_atender_consola;
		int socketCliente = esperar_cliente(server_fd, logger);

		t_datosCrearPCB *datos = malloc(sizeof(t_datosCrearPCB));
		t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));
		datos->instrucciones = instrucciones;
		
		datos->instrucciones = recibir_instruciones_desde_consola(socketCliente);
		datos->socket = socketCliente;
		
		if(!enviarMensaje(socketCliente, "Llegaron las instrucciones"))
			log_error(logger, "Error al enviar el mensaje");

		pthread_create(&hilo_atender_consola, NULL, (void *)crear_pcb, (void*)datos);
		pthread_detach(hilo_atender_consola);
	}
}

t_instrucciones *recibir_instruciones_desde_consola(int cliente_fd){

	log_debug(logger, "Recibiendo instrucciones desde consola...");
	
	int cantidad_de_instrucciones = 0;
	t_tipoInstruccion instruccion;
	t_instruccion* instruccionRecibida;
	t_instrucciones* instruccionesRecibidas = malloc(sizeof(t_instrucciones));
	instruccionesRecibidas->listaInstrucciones = list_create();
	
	
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
			log_error(logger, "long wait: %d", instruccionRecibida->longitud_cadena);
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
	
	t_pcb *pcb 					   = malloc(sizeof(t_pcb));
	t_contextoEjecucion *contexto  = malloc(sizeof(t_contextoEjecucion));
	t_instrucciones *instrucciones = malloc(sizeof(t_instrucciones));

	pcb->contexto = contexto;
	pcb->contexto->instrucciones = instrucciones;

	pthread_mutex_lock(&PID);
	pcb->contexto->pid              = ++PID_PCB;
	pthread_mutex_unlock(&PID);
	pcb->contexto->socket           = datosPCB->socket;
	pcb->contexto->program_counter  = 0;

	pcb->contexto->instrucciones->cantidadInstrucciones = datosPCB->instrucciones->cantidadInstrucciones;
	pcb->contexto->instrucciones->listaInstrucciones = list_create();
	pcb->contexto->instrucciones->listaInstrucciones = list_duplicate(datosPCB->instrucciones->listaInstrucciones);
	
	pcb->contexto->tablaDeSegmentos = list_create();
	agregar_segmento_a_pcb(); //agregar el segmento 0 recibido de memoria
	//pcb-> registrosCPU;
	pcb->estimacion_anterior       = 10000;
	pcb->real_anterior             = 0;
    //pcb->llegadaReady            = time(NULL);
	pcb->taap                      = list_create();

	pasar_a_new(pcb);

	sem_post(&CantPCBNew);
}