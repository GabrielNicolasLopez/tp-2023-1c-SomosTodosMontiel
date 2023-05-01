#include "kernel.h"

int main(void)
{

	// Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);

	log_info(logger, "INICIANDO KERNEL...");

	// Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion();
	sem_init(&multiprogramacion, 0, configuracionKernel->GRADO_MAX_MULTIPROGRAMACION);

	// Inicializo las listas y semaforos
	iniciar_listas_y_semaforos();

	// cargarRecursos();

	crear_hilos_kernel();
	liberar_listas_y_semaforos();
	log_error(logger, "termine de ejecutar");
}

void crear_hilos_kernel()
{
	pthread_t hiloConsola, hiloCPU, hiloFilesystem, hiloMemoria, planifCortoPlazo, planiLargoPlazo;

	sem_init(&cantPCB, 0, 0);
	

	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	pthread_create(&hiloCPU, NULL, (void *)crear_hilo_cpu, NULL);
	//pthread_create(&hiloFilesystem, NULL, (void *)crear_hilo_filesystem, NULL);
	//pthread_create(&hiloMemoria, NULL, (void *)crear_hilo_memoria, NULL);
	pthread_create(&planiLargoPlazo, NULL, (void *)planiLargoPlazo, NULL);
	pthread_create(&planifCortoPlazo, NULL, (void *)planifCortoPlazo, NULL);

	pthread_detach(planifCortoPlazo);
	pthread_detach(planiLargoPlazo);
	//pthread_detach(hiloConsola);
	pthread_detach(hiloCPU);
	//pthread_detach(hiloFilesystem);
	//pthread_detach(hiloMemoria);

	pthread_join(hiloConsola, NULL);
}


// void planiLargoPlazo(){
// 	while (1){
// 		//Espera a que haya alguna PCB disponible para pasarla a READY
// 		sem_wait(&cantPCB);
// 		agregar_pcb();
// 	}
// }

// void planifCortoPlazo(){
// 	while (1){
// 		sem_wait(&cantPCBReady);
// 		log_info(logger, "Llego pcb a plani corto plazo");
// 		t_tipo_algoritmo algoritmo = obtenerAlgoritmo();

// 		switch (algoritmo){
// 		case FIFO:
// 			log_debug(logger, "Implementando algoritmo FIFO");
// 			log_debug(logger, " Cola Ready FIFO:");
// 			cargarListaReadyIdPCB(LISTA_READY);
// 			implementar_fifo();

// 			break;
// 		case HRRN:
// 			log_debug(logger, "Implementando algoritmo RR");
// 			log_debug(logger, " Cola Ready RR:");
// 			cargarListaReadyIdPCB(LISTA_READY);
// 			implementar_hrrn();

// 			break;
// 		default:
// 			break;
// 		}
// 	}
// }

void implementar_fifo(){

}

void implementar_hrrn(){
	
}

// t_tipo_algoritmo obtenerAlgoritmo(){
// 	char *algoritmoConfig = configuracionKernel->ALGORITMO_PLANIFICACION;
// 	t_tipo_algoritmo algoritmo;

// 	if(!strcmp(algoritmo, "FIFO"))
// 		algoritmo = FIFO;
// 	else if(!strcmp(algoritmo, "RR"))
// 		algoritmo = HRRN;
// 	else
// 		log_error(logger, "ALGORITMO ESCRITO INCORRECTAMENTE");
// 	return algoritmo;
// }

void agregar_pcb(){
	sem_wait(&multiprogramacion);

	log_info(logger, "Agregando un pcb a lista ready...");

	//Bloqueo la lista de NEW para sacar una pcb
	pthread_mutex_lock(&listaNew);
	//Saco el 1er elemento de la lista de new (ESO NO TIENE NADA QUE VER CON FIFO. SIEMPRE SE USA FIFO PARA SACAR PCBS DE NEW)
	t_pcb *pcb = list_remove(LISTA_NEW, 0); 
	//Desbloqueo la lista para que la puedan usar los otros hilos
	pthread_mutex_unlock(&listaNew); 

	// solicito que memoria inicialice sus estructuras
	/*pthread_mutex_lock(&mutex_conexion_memoria);
	serializarPCB(conexionMemoria, pcb, ASIGNAR_RECURSOS);
	pthread_mutex_unlock(&mutex_conexion_memoria);
	free(pcb);
	log_info(logger, "Envio recursos a memoria\n");

	// memoria me devuelve el pcb modificado
	pthread_mutex_lock(&mutex_conexion_memoria);
	t_paqueteActual *paquete = recibirPaquete(conexionMemoria);
	// pthread_mutex_unlock(&mutex_conexion_memoria);
	log_info(logger, "Recibo recursos de memoria\n");
	if (paquete == NULL)
	{
		log_error(logger, "Paquete nulo\n");
	}
	else
	{
		log_info(logger, "Paquete no nulo\n");
	}
	// pthread_mutex_lock(&mutex_conexion_memoria);
	pcb = deserializoPCB(paquete->buffer);
	pthread_mutex_unlock(&mutex_conexion_memoria);*/

	// imprimirInstruccionesYSegmentos(*(pcb->informacion));
	/*for (int i = 0; i < list_size(pcb->tablaSegmentos); i++)
	{
		t_tabla_segmentos *tablaSegmento = malloc(sizeof(t_tabla_segmentos));

		t_tabla_segmentos *segmento = list_get(pcb->tablaSegmentos, i);
		log_info(logger,"El id del segmento es: %d\n", segmento->id);

		log_info(logger,"El id de la tabla es: %d\n", segmento->indiceTablaPaginas);
	}*/

	pasar_a_ready(pcb);

	//log_debug(logger, "Estado Anterior: NEW , proceso id: %d", pcb->pid);
	//log_debug(logger, "Estado Actual: READY , proceso id: %d", pcb->pid);
	// Cambio de estado
	log_info(logger, "Cambio de Estado: PID %d - Estado Anterior: NEW, Estado Actual: READY", pcb->pid);
	log_info(logger, "Cant de elementos de ready: %d\n", list_size(LISTA_READY));

	//Aumento el semaforo de cantidad de PCBs en ready 
	sem_post(&cantPCBReady);
	//log_info(logger, "Envie a memoria los recursos para asignar");
}

void crear_hilo_memoria(){
	// Me conecto a memoria
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA, logger);
	log_info(logger, "Hola, me conecté a memoria");
	while (1)
	{
	}
}

void crear_hilo_filesystem(){
	// Me conecto a filesystem
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM, logger);
	log_info(logger, "Hola, me conecté a filesystem");
	while (1)
	{
	}
}

void crear_hilo_cpu(){
	// Me conecto a cpu
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	log_info(logger, "Hola, me conecté a cpu");
	while (1)
	{
		t_paquete *paqueteContextoEjecucion = crear_paquete_contexto_ejecucion(list_get(LISTA_NEW, 0));

		//log_info(logger, "CONSOLA-KERNEL");
		//enviar_mensaje("HOLA SOY LA CONSOLA", conexionKernel);
		
		//Enviamos el paquete
		enviar_paquete(paqueteContextoEjecucion, conexion_con_cpu);
		//Borramos el paquete
		eliminar_paquete(paqueteContextoEjecucion);
	}
}

void crear_hilo_consola(){
	int server_fd = iniciar_servidor("127.0.0.1", configuracionKernel->PUERTO_ESCUCHA, logger);
	log_info(logger, "Kernel listo para recibir clientes consola");

	while (1){
		pthread_t hilo_atender_consola;
		int socketCliente = esperar_cliente(server_fd, logger);
		
		int cod_op = recibir_operacion(socketCliente);

		t_datosCrearPCB *datos = malloc(sizeof(t_datosCrearPCB));
		datos->instrucciones = recibir_informacion(socketCliente);
		datos->socket = socketCliente;

		if(!enviarMensaje(socketCliente, "Llegaron las instrucciones"))
			log_error(logger, "Error al enviar el mensaje");

		//Envio un paquete para que consola finalice
		t_paquete* paquete = crear_paquete();
		paquete->codigo_operacion = TERMINAR_CONSOLA;
		agregar_a_paquete(paquete, NULL, 0);
		enviar_paquete(paquete, socketCliente);
		log_info(logger, "Se terminó una consola");

		pthread_create(&hilo_atender_consola, NULL, (void *)crear_pcb, (void*)datos);
		pthread_detach(hilo_atender_consola);
	}
}

void crear_pcb(void *datos){
	t_datosCrearPCB *datosPCB = (t_datosCrearPCB *)datos;
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pthread_mutex_lock(&PID);
	pcb->pid                  = ++PID_PCB;
	pthread_mutex_unlock(&PID);
	pcb->socket               = datosPCB->socket;
	pcb->program_counter      = 0;
	pcb->instrucciones        = &datosPCB->instrucciones;
	pcb->tablaDeSegmentos     = list_create();
	//pcb-> registrosCPU;
	pcb->estimacionProxRafaga = time(NULL);
	pcb->llegadaReady         = time(NULL);
	pcb->taap                 = list_create();

	//Agrego la pcb creada a la lista de procesos NEW
	//list_add(LISTA_NEW, pcb);
	pasar_a_new(pcb);

	log_info(logger, "Creación de Proceso: se crea el proceso %d en NEW", pcb->pid); 
	log_info(logger, "Cant de elementos de new: %d", list_size(LISTA_NEW));

	sem_post(&cantPCB);
}

void pasar_a_new(t_pcb *pcb){
	pthread_mutex_lock(&listaNew);
	list_add(LISTA_NEW, pcb);
	pthread_mutex_unlock(&listaNew);
	log_debug(logger, "Paso a NEW el proceso %d", pcb->pid);
}

void pasar_a_ready(t_pcb *pcb){
	pthread_mutex_lock(&listaReady);
	list_add(LISTA_READY, pcb);
	pthread_mutex_unlock(&listaReady);
	log_debug(logger, "Paso a READY el proceso %d", pcb->pid);
}

void iniciar_listas_y_semaforos(){
	LISTA_NEW = list_create();
	LISTA_READY = list_create();
	LISTA_EXEC = list_create();
	LISTA_BLOCKED = list_create();
	LISTA_EXIT  = list_create();
}

//Supongo que se tiene que liberar en algun momento
void liberar_listas_y_semaforos(){}









int enviarMensaje(int socket, char *msj){
	size_t size_stream;
	void *stream = serializarMensaje(msj, &size_stream);
	return enviarStream(socket, stream, size_stream);
}

void *serializarMensaje(char *msj, size_t *size_stream){

	*size_stream = strlen(msj) + 1;

	void *stream = malloc(sizeof(*size_stream) + *size_stream);

	memcpy(stream, size_stream, sizeof(*size_stream));
	memcpy(stream + sizeof(*size_stream), msj, *size_stream);

	*size_stream += sizeof(*size_stream);

	return stream;
}

int enviarStream(int socket, void *stream, size_t stream_size){

	if (send(socket, stream, stream_size, 0) == -1)
	{
		free(stream);
		return 0;
	}

	free(stream);
	return 1;
}

t_kernel_config *leerConfiguracion(){

	// Creo el config para leer IP y PUERTO
	t_config *config = config_create(CONFIG_PATH);

	// Creo el archivo config
	t_kernel_config *configuracionKernel = malloc(sizeof(t_kernel_config));

	// Leo los datos del config para que kernel funcione como servidor (no tiene IP para funcionar como servido)
	configuracionKernel->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
	configuracionKernel->ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configuracionKernel->ESTIMACION_INICIAL = config_get_string_value(config, "ESTIMACION_INICIAL");
	configuracionKernel->HRRN_ALFA = config_get_int_value(config, "HRRN_ALFA");
	configuracionKernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
	configuracionKernel->RECURSOS = config_get_array_value(config, "RECURSOS");
	configuracionKernel->INSTANCIAS_RECURSOS = config_get_array_value(config, "INSTANCIAS_RECURSOS");

	// Leo los datos del config para que kernel se conecte al resto de modulo
	configuracionKernel->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracionKernel->IP_FILESYSTEM = config_get_string_value(config, "IP_FILESYSTEM");
	configuracionKernel->IP_CPU = config_get_string_value(config, "IP_CPU");

	configuracionKernel->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracionKernel->PUERTO_FILESYSTEM = config_get_string_value(config, "PUERTO_FILESYSTEM");
	configuracionKernel->PUERTO_CPU = config_get_string_value(config, "PUERTO_CPU");

	return configuracionKernel;
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/

void cargarRecursos()
{

	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++)
	{
		// log_info(logger, "Tamaño %d\n", size_char_array(configuracionKernel.RECURSOS));
		char *recursoNuevo = configuracionKernel->RECURSOS[i];
		t_recurso *recurso = malloc(sizeof(t_recurso));

		recurso->nombre = recursoNuevo;
		recurso->lista_block = list_create();
		int var_instancias_recursos = atoi(configuracionKernel->INSTANCIAS_RECURSOS[i]);
		recurso->instancias_recursos = var_instancias_recursos;
		// el segundo valor del sem_init es entre cuantos hilos se comparte el semaforo, para id
		// deberia ser entre todos los hilos que va a tener kernel abiertos
		// sem_init(&recurso->contador_bloqueo, 0, var_instancias_recursos);
		// pthread_mutex_init(&recurso->mutex_lista_blocked, NULL);

		// log_info(logger, "recursos %s , %d \n", recurso->recurso, recurso->tiempoEjecucion);
		// agregrar_recurso(recurso);
		log_info(logger, "Se cargo un recurso de nombre: %s y instancias %i", recurso->nombre, recurso->instancias_recursos);
	}
}

// void agregrar_recurso(t_recurso *recurso)
// {
// 	pthread_mutex_lock(&mutex_lista_blocked);
// 	list_add(LISTA_BLOCKED, recurso);
// 	pthread_mutex_unlock(&mutex_lista_blocked);
// }

/*t_instrucciones recibir_informacion(int cliente_fd){
	int size;
	void *buffer = recibir_buffer(&size, cliente_fd);
	t_instrucciones instrucciones;
	int offset = 0;
	memcpy(&(instrucciones.cantidadInstrucciones), buffer + offset, sizeof(uint32_t));
	//printf("la cantidad de instrucciones es: %d", instrucciones.cantidadInstrucciones);
	offset += sizeof(uint32_t);
	instrucciones.listaInstrucciones = list_create();
	t_instruccion *instruccion;

	int k = 0;

	while (k < instrucciones.cantidadInstrucciones){
		instruccion = malloc(sizeof(t_instruccion));
		// El tipo de instruccion
		memcpy(&instruccion->tipo, buffer + offset, sizeof(t_tipoInstruccion));
		offset += sizeof(t_tipoInstruccion);
		// Los registros
		memcpy(&instruccion->registros[0], buffer + offset, sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(&instruccion->registros[1], buffer + offset, sizeof(t_registro));
		offset += sizeof(t_registro);
		// Los int
		memcpy(&instruccion->paramIntA, buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&instruccion->paramIntB, buffer + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		// Los 3 char*
		memcpy(&instruccion->recurso, buffer + offset, sizeof(instruccion->recurso));
		offset += sizeof(instruccion->recurso);
		memcpy(&instruccion->cadenaRegistro, buffer + offset, sizeof(instruccion->cadenaRegistro));
		offset += sizeof(instruccion->cadenaRegistro);
		memcpy(&instruccion->nombreArchivo, buffer + offset, sizeof(instruccion->nombreArchivo));
		offset += sizeof(instruccion->nombreArchivo);

		list_add(instrucciones.listaInstrucciones, instruccion);
		k++;
	}

	free(buffer);
	return instrucciones;
}*/

t_paquete *crear_paquete_contexto_ejecucion(t_pcb *pcb){	
	log_info(logger,"Empiezo a serializar contexto de ejecucion");
	t_buffer *buffer = malloc(sizeof(t_buffer));
	//log_error(logger, "buffer: %d", sizeof(buffer));
	/*buffer->size = sizeof(uint32_t)*2 //Cantidad de instrucciones
				   + calcularSizeListaInstrucciones(instrucciones); // Peso de las instrucciones*/

	buffer->size = 
	sizeof(uint32_t) +	//pid
	sizeof(uint32_t) +	//program_counter
	sizeof(uint32_t) + calcularSizeListaInstrucciones(pcb->instrucciones) + //t_instrucciones
	calcularSizeTablaSegmentos(pcb->tablaDeSegmentos) +	//tabla de segmentos
	calcularSizeRegistroCPU(pcb->registrosCPU)	// t_registrosCPU
	;
	//log_error(logger, "peso lista: %d", calcularSizeListaInstrucciones(instrucciones));
	void *stream = malloc(buffer->size);
	//log_error(logger, "tamaño: %d", sizeof(stream));

	int offset = 0; // Desplazamiento
	memcpy(stream + offset, &(pcb->pid), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(pcb->program_counter), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(pcb->instrucciones->cantidadInstrucciones), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	//log_error(logger, "tamaño: %d", sizeof(stream));
	//log_error(logger, "cantidadInstrucciones: %d", instrucciones->cantidadInstrucciones);
	
	// Serializa las instrucciones
	int i = 0;

	while (i < list_size(pcb->instrucciones->listaInstrucciones)){
		t_instruccion* instrucccion = list_get(pcb->instrucciones->listaInstrucciones, i);
		memcpy(stream + offset,&instrucccion->tipo, sizeof(t_tipoInstruccion));
		offset += sizeof(t_tipoInstruccion);
		memcpy(stream + offset,&instrucccion->registros[0], sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(stream + offset,&instrucccion->registros[1], sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(stream + offset,&instrucccion->paramIntA, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset,&instrucccion->paramIntB, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset,&instrucccion->recurso, sizeof(instrucccion->recurso));
		offset += sizeof(instrucccion->recurso);
		memcpy(stream + offset,&instrucccion->cadenaRegistro, sizeof(instrucccion->cadenaRegistro));
		offset += sizeof(instrucccion->cadenaRegistro);
		memcpy(stream + offset,&instrucccion->nombreArchivo, sizeof(instrucccion->nombreArchivo));
		offset += sizeof(instrucccion->nombreArchivo);
		i++;
	}

	t_registrosCPU* registrosCPU = pcb->registrosCPU;
	memcpy(stream + offset,&registrosCPU->registro, sizeof(t_registro));
	offset += sizeof(t_registro);
	memcpy(stream + offset,&registrosCPU->registroE, sizeof(t_registroE));
	offset += sizeof(t_registroE);
	memcpy(stream + offset,&registrosCPU->registroR, sizeof(t_registroR));
	offset += sizeof(t_registroR);


	int j=0;

	while (j < list_size(pcb->tablaDeSegmentos)){
		entrada_tablaDeSegmentos* entrada_tablaDeSegmentos = list_get(pcb->tablaDeSegmentos, j);
		memcpy(stream + offset,&entrada_tablaDeSegmentos->prueba, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		i++;
	}

	
	//log_error(logger, "stream: %d", sizeof((*(t_instruccion*)stream)));
	//log_error(logger, "offset: %d", sizeof(offset));

	buffer->stream = stream; // Payload

	// free(informacion->instrucciones);
	// free(informacion->segmentos);

	// lleno el paquete
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW;
	paquete->buffer = buffer;
	return paquete;
}

int calcularSizeListaInstrucciones(t_instrucciones *instrucciones){
	int total = 0;
	for (int i = 0 ; i < list_size(instrucciones->listaInstrucciones); i++){
		
		t_instruccion* instruccion = list_get(instrucciones->listaInstrucciones,i);
		total += sizeof(t_tipoInstruccion);
		total += sizeof(t_registro) * 2;
		total += sizeof(uint32_t) * 2;
		total += sizeof(instruccion->recurso);
		total += sizeof(instruccion->nombreArchivo);
		total += sizeof(instruccion->cadenaRegistro);
		//log_info(logger, "total: %d", sizeof(total));
	}
	return total;
}