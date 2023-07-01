#include "cpu.h"

t_cpu_config* configuracion_cpu;

int conexion_con_memoria;

int k=0;

int main(int argc, char **argv){
	if (argc < 2) 
    {
        fprintf(stderr, "Se esperaba: %s [CONFIG_PATH]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	char *CONFIG_PATH = argv[1];

	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG); if (!logger) return EXIT_FAILURE;

	log_info(logger, "INICIANDO CPU...");
	
	t_config* config = config_create(CONFIG_PATH);
	configuracion_cpu = leer_configuracion(config);

	crear_hilos_cpu();

	// ANTES DE FINALIZAR EL PROCESO LIBERAR MEMORIA:
	config_destroy(config);
	free(configuracion_cpu);
}

// OBTENEMOS LA CONFIGURACION DEL PROCESO
t_cpu_config* leer_configuracion(t_config* config){

	// Creo el config para leer IP y PUERTO

	t_cpu_config* configuracion = malloc(sizeof(t_cpu_config));

	configuracion -> puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	configuracion -> ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	configuracion -> puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracion -> retardo_instruccion = config_get_int_value(config, "RETARDO_INSTRUCCION");

	return configuracion;
}

// CREA LOS HILOS QUE VA A USAR LA CPU
void crear_hilos_cpu()
{
	pthread_t hiloKernel, hiloMemoria;

	pthread_create(&hiloMemoria, NULL, (void *) hilo_memoria, NULL);
	pthread_create(&hiloKernel, NULL, (void *) hilo_kernel, NULL);

	//pthread_join(hiloMemoria, NULL);	
	pthread_join(hiloKernel, NULL);
}

// *** HILO KERNEL ***
void hilo_kernel(){
	int server_fd_kernel = iniciar_servidor("127.0.0.1", configuracion_cpu -> puerto_escucha);
	log_error(logger, "CPU listo para recibir clientes del Kernel");
    int cliente_fd_kernel = esperar_cliente(server_fd_kernel); // esperamos un proceso para ejecutar

	uint8_t handshake = stream_recv_header(cliente_fd_kernel);
	stream_recv_empty_buffer(cliente_fd_kernel);
	log_info(logger, "handshake kernel = 3: %d", handshake);
	if (handshake == HANDSHAKE_kernel) {
		log_info(logger, "Se envia handshake ok continue a kernel");
		stream_send_empty_buffer(cliente_fd_kernel, HANDSHAKE_ok_continue);

		log_debug(logger, "CPU SE CONECTO CON KERNEL");

		t_contextoEjecucion* contexto_ejecucion = malloc(sizeof(t_contextoEjecucion));
		bool enviamos_CE_al_kernel;
		
		while(1){
			contexto_ejecucion = recibir_ce_de_kernel(cliente_fd_kernel);
			enviamos_CE_al_kernel = false;
			while(contexto_ejecucion && !enviamos_CE_al_kernel)
			{
				ciclo_instruccion(contexto_ejecucion, cliente_fd_kernel, &enviamos_CE_al_kernel);
			}
			//contexto_de_ejecucion_destroy(contexto_ejecucion);
		}

	} else {
		stream_send_empty_buffer(cliente_fd_kernel, HANDSHAKE_error);
	}

}

// *** HILO MEMORIA ***
void hilo_memoria(){
    
	// Me conecto a filesystem
	/*int conexion_con_memoria = crear_conexion(configuracion_cpu->ip_memoria, configuracion_cpu->puerto_memoria);

	if (conexion_con_memoria == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON FS. FINALIZANDO CPU...");
		//kernel_destroy(configuracionKernel, logger);
		//exit(-1);
	}

	stream_send_empty_buffer(conexion_con_memoria, HANDSHAKE_cpu);
    uint8_t memoriaResponse = stream_recv_header(conexion_con_memoria);
	stream_recv_empty_buffer(conexion_con_memoria);

    if (memoriaResponse != HANDSHAKE_ok_continue)
	{
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        //kernel_destroy(configuracionKernel, logger);
        //exit(-1);
    }
	
	log_debug(logger, "CPU SE CONECTO CON Memoria");
	
	while (1){}*/
	
}

void enviar_cym_a_kernel(t_motivoDevolucion motivo, t_contextoEjecucion *contextoEjecucion, int cliente_fd_kernel){

	log_debug(logger, "Enviando cym a kernel");
	
	t_buffer* cym_a_enviar = buffer_create();

	// ----------- DATOS DE MOTIVO DE DEVOLUCION -----------

	//Motivo = tipo de instruccion
    buffer_pack(cym_a_enviar, &motivo.tipo, sizeof(t_tipoInstruccion));
	//log_error(logger, "md TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	
	//Cantidad entero = numero entero
	buffer_pack(cym_a_enviar, &motivo.cant_int, sizeof(uint32_t));
	//log_error(logger, "md TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

	//Cantidad entero = numero entero
	buffer_pack(cym_a_enviar, &motivo.cant_intB, sizeof(uint32_t));
	//log_error(logger, "md TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	
	//Longitud de la cadena
	buffer_pack(cym_a_enviar, &motivo.longitud_cadena, sizeof(uint32_t));
	//log_error(logger, "md TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	
	//Cadena
	buffer_pack(cym_a_enviar, &motivo.cadena, motivo.longitud_cadena);
	//log_error(logger, "md TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

	// ----------- DATOS DE CONTEXTO DE EJECUCIÓN -----------

	//Socket
	buffer_pack(cym_a_enviar, &contextoEjecucion->socket, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	//PID
	buffer_pack(cym_a_enviar, &contextoEjecucion->pid, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	//PC
	buffer_pack(cym_a_enviar, &contextoEjecucion->program_counter, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

	//Instrucciones
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	for(int i=0; i<contextoEjecucion->instrucciones->cantidadInstrucciones; i++){
		instruccion = list_get(contextoEjecucion->instrucciones->listaInstrucciones, i);
		//log_debug(logger, "obtuve la instruccion: %d", i+1);
		buffer_pack(cym_a_enviar, &instruccion->tipo, sizeof(t_tipoInstruccion));
		//log_error(logger, "TAMAÑO DEL BUFFER I%d", cym_a_enviar->size);

		if (instruccion->tipo == SET){
			buffer_pack(cym_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			//log_error(logger, "longitud cadena: %d", instruccion->longitud_cadena);
			//log_error(logger, "cadena: %.4s", instruccion->cadena);
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == MOV_IN){
			buffer_pack(cym_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == MOV_OUT){
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, &instruccion->registro, sizeof(t_registro));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == IO){
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_OPEN){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_CLOSE){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_SEEK){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_READ){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_WRITE){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == F_TRUNCATE){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == WAIT){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);	
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == SIGNAL){
			buffer_pack(cym_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, instruccion->cadena, instruccion->longitud_cadena);		
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
				
		}
		else if (instruccion->tipo == CREATE_SEGMENT){
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == DELETE_SEGMENT){
			buffer_pack(cym_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == YIELD){
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
		else if (instruccion->tipo == EXIT){
			//log_error(logger, "ins TAMAÑO DEL BUFFER %d", cym_a_enviar->size);

		}
	}

	//Registros C, E y R
	buffer_pack(cym_a_enviar, contextoEjecucion->registrosCPU->registroC, sizeof(t_registroC));
	log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	buffer_pack(cym_a_enviar, contextoEjecucion->registrosCPU->registroE, sizeof(t_registroE));
	log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	buffer_pack(cym_a_enviar, contextoEjecucion->registrosCPU->registroR, sizeof(t_registroR));
	log_error(logger, "TAMAÑO DEL BUFFER %d", cym_a_enviar->size);
	
	
	uint8_t header = CYM;
	stream_send_buffer(cliente_fd_kernel, header, cym_a_enviar);
	log_error(logger, "Tamaño del cym enviado a kernel %d", cym_a_enviar->size);

    buffer_destroy(cym_a_enviar);
}

t_contextoEjecucion* recibir_ce_de_kernel(int cliente_fd_kernel){

	log_debug(logger, "Esperando ce de kernel");

	uint8_t handshake = stream_recv_header(cliente_fd_kernel);

	log_debug(logger, "header: %d", handshake);

    t_buffer* ce_recibido = buffer_create();
	t_contextoEjecucion* contextoEjecucion = malloc(sizeof(t_contextoEjecucion));
	t_instrucciones *inst = malloc(sizeof(t_instrucciones));
	
	t_registrosCPU *registros = malloc(sizeof(t_registrosCPU));
	t_registroC *registroC    = malloc(sizeof(t_registroC));
	t_registroE *registroE    = malloc(sizeof(t_registroE));
	t_registroR *registroR    = malloc(sizeof(t_registroR));
	
	contextoEjecucion->instrucciones = inst;
	contextoEjecucion->instrucciones->listaInstrucciones = list_create();
	contextoEjecucion->registrosCPU = registros;
	contextoEjecucion->registrosCPU->registroC = registroC;
	contextoEjecucion->registrosCPU->registroE = registroE;
	contextoEjecucion->registrosCPU->registroR = registroR;

    stream_recv_buffer(cliente_fd_kernel, ce_recibido);
	log_error(logger, "Tamaño del CE recibido de kernel %d", ce_recibido->size);

	//Socket
	buffer_unpack(ce_recibido, &contextoEjecucion->socket, sizeof(uint32_t));
	//log_debug(logger, "socket: %d", contextoEjecucion->socket);
	
	//PID
	buffer_unpack(ce_recibido, &contextoEjecucion->pid, sizeof(uint32_t));
	//log_debug(logger, "pid: %d", contextoEjecucion->pid);
	
	//PC
	buffer_unpack(ce_recibido, &contextoEjecucion->program_counter, sizeof(uint32_t));
	//log_debug(logger, "program_counter: %d", contextoEjecucion->program_counter);

	
	t_tipoInstruccion instruccion;
	t_instruccion* instruccionRecibida;
	int cantidad_de_instrucciones = 0;
	//Empiezo a desempaquetar las instrucciones
	buffer_unpack(ce_recibido, &instruccion, sizeof(t_tipoInstruccion));

	while(instruccion != EXIT){
		instruccionRecibida = malloc(sizeof(t_instruccion));

		instruccionRecibida->tipo = instruccion;
		//log_debug(logger, "tipo de instruccion: %d", instruccion);
	
		if(instruccion == DELETE_SEGMENT)
			//Numero de segmento
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		if(instruccion == CREATE_SEGMENT){
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == SIGNAL){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == WAIT){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == F_TRUNCATE){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_WRITE){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_READ){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Parametro B
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if(instruccion == F_SEEK){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == F_CLOSE){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == F_OPEN){
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == IO){
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == SET){
			//Registro
			buffer_unpack(ce_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			//Longitud cadena
			buffer_unpack(ce_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			//Cadena
			buffer_unpack(ce_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if(instruccion == MOV_IN){
			//Registro
			buffer_unpack(ce_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if(instruccion == MOV_OUT){
			//Parametro A
			buffer_unpack(ce_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			//Registro
			buffer_unpack(ce_recibido, &instruccionRecibida->registro, sizeof(t_registro));
		}

		//Agregamos la instruccion recibida a la lista de instrucciones
		list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
		cantidad_de_instrucciones++;

		//Obtengo una nueva instruccion
		buffer_unpack(ce_recibido, &instruccion, sizeof(t_tipoInstruccion));
	}
	instruccionRecibida = malloc(sizeof(t_instruccion));
	//Instruccion EXIT
	instruccionRecibida->tipo = instruccion;
	//log_debug(logger, "tipo de instruccion: %d", instruccion);
	list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
	cantidad_de_instrucciones++;
	//Asignamos la cantidad de instrucciones
	contextoEjecucion->instrucciones->cantidadInstrucciones = cantidad_de_instrucciones;

	log_debug(logger, "cant inst recibidas: %d", contextoEjecucion->instrucciones->cantidadInstrucciones);

	//Registros C, E y R	
	buffer_unpack(ce_recibido, contextoEjecucion->registrosCPU->registroC, sizeof(t_registroC));
	log_error(logger, "TAMAÑO DEL BUFFER %d", ce_recibido->size);
	buffer_unpack(ce_recibido, contextoEjecucion->registrosCPU->registroE, sizeof(t_registroE));
	log_error(logger, "TAMAÑO DEL BUFFER %d", ce_recibido->size);
	buffer_unpack(ce_recibido, contextoEjecucion->registrosCPU->registroR, sizeof(t_registroR));
	log_error(logger, "TAMAÑO DEL BUFFER %d", ce_recibido->size);

	sem_trywait

    buffer_destroy(ce_recibido);

    return contextoEjecucion;
}