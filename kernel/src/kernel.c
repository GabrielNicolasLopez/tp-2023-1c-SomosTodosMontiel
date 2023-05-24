#include "kernel.h"

int main(void){

	// Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);

	log_debug(logger, "INICIANDO KERNEL...");

	// Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion();
	// Inicializo las listas y semaforos
	iniciar_listas_y_semaforos();

	// cargarRecursos();
	//Creo hilos de kernel para todas las conexiones y planificadores
	crear_hilos_kernel();

	//Libero memoria
	liberar_listas_y_semaforos();

	log_error(logger, "KERNEL TERMINO DE EJECUTAR...");
}

void crear_hilos_kernel(){
	pthread_t hiloConsola, hiloCPU, hiloPlaniCortoPlazo, hiloPlaniLargoPlazo;
	//, hiloFilesystem, hiloMemoria, , ;

	//Hilos de modulos
	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	pthread_create(&hiloCPU, NULL, (void *)crear_hilo_cpu, NULL);

	//Hilos de planificadores
	pthread_create(&hiloPlaniCortoPlazo, NULL, (void *)planiCortoPlazo, NULL);
	pthread_create(&hiloPlaniLargoPlazo, NULL, (void *)planiLargoPlazo, NULL);

	
	pthread_detach(hiloPlaniCortoPlazo);
	pthread_detach(hiloPlaniLargoPlazo);

	pthread_join(hiloConsola, NULL);
	
	//pthread_create(&hiloFilesystem, NULL, (void *)crear_hilo_filesystem, NULL);
	//pthread_create(&hiloMemoria, NULL, (void *)crear_hilo_memoria, NULL);

	//pthread_detach(hiloCPU);
	//pthread_detach(hiloFilesystem);
	//pthread_detach(hiloMemoria);
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
	pcb->estimacion_anterior  = 10000;
	pcb->real_anterior        = 0;
    //pcb->llegadaReady       = time(NULL);
	pcb->taap                 = list_create();

	pasar_a_new(pcb);

	//log_info(logger, "Creación de Proceso: se crea el proceso %d en NEW", pcb->pid); 
	//log_info(logger, "Cant de elementos de new: %d", list_size(LISTA_NEW));

	sem_post(&CantPCBNew);
}

void crear_hilo_cpu(){
	// Me conecto a cpu
	//int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	//log_info(logger, "Hola, me conecté a cpu");
	int v_mp;
	while (1){
		//t_paquete *paqueteContextoEjecucion = crear_paquete_contexto_ejecucion(list_get(LISTA_NEW, 0));

		//log_info(logger, "CONSOLA-KERNEL");
		//enviar_mensaje("HOLA SOY LA CONSOLA", conexionKernel);
		
		//Enviamos el paquete
		//enviar_paquete(paqueteContextoEjecucion, conexion_con_cpu);
		//Borramos el paquete
		//eliminar_paquete(paqueteContextoEjecucion);

		sem_wait(&pasar_pcb_a_CPU);

		pthread_mutex_lock(&listaExec);
		t_pcb* pcb = list_remove(LISTA_EXEC, 0);
		pthread_mutex_unlock(&listaExec);

		log_info(logger, "Proceso PID: %d EMPIEZA A EJECUTAR", pcb->pid);
		sleep(5);
		log_info(logger, "Proceso PID: %d TERMINAR DE EJECUTAR", pcb->pid);

		//Envio un paquete para que consola finalice
		t_paquete* paquete = crear_paquete();
		paquete->codigo_operacion = TERMINAR_CONSOLA;
		agregar_a_paquete(paquete, NULL, 0);
		enviar_paquete(paquete, pcb->socket);
		log_error(logger, "FINALIZO EL PROCESO: %d", pcb->pid);
		pasar_a_exit(pcb);
		
		sem_post(&CPUVacia);
		sem_post(&multiprogramacion);
		sem_getvalue(&multiprogramacion, &v_mp);
		log_debug(logger, "MP=%d", v_mp);;
	}
}

void crear_hilo_memoria(){
	// Me conecto a memoria
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA, logger);
	log_info(logger, "Hola, me conecté a memoria");
	while (1){}
}

void crear_hilo_filesystem(){
	// Me conecto a filesystem
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM, logger);
	log_info(logger, "Hola, me conecté a filesystem");
	while (1){}
}

void planiLargoPlazo(){
 	while (1){
 		//Espera a que haya alguna PCB disponible para pasarla a READY
 		sem_wait(&CantPCBNew);
		//sem_wait(&multiprogramacion); NO SE SI EL WAIT DEBERIA IR ACA O ADENTRO DE LA FUNCION DONDE AHORA ESTÁ
 		agregar_pcb_a_ready();
 	}
}

void agregar_pcb_a_ready(){
	int v_mp;
	sem_getvalue(&multiprogramacion, &v_mp);
	log_debug(logger, "MP=%d", v_mp);
	sem_wait(&multiprogramacion);
	sem_getvalue(&multiprogramacion, &v_mp);
	log_debug(logger, "MP=%d", v_mp);
	//Bloqueo la lista de NEW para sacar una pcb
	pthread_mutex_lock(&listaNew);
	//Saco el 1er elemento de la lista de new (ESO NO TIENE NADA QUE VER CON FIFO. SIEMPRE SE USA FIFO PARA SACAR PCBS DE NEW)
	t_pcb *pcb = list_remove(LISTA_NEW, 0); 
	//Desbloqueo la lista para que la puedan usar los otros hilos
	pthread_mutex_unlock(&listaNew); 

	pasar_a_ready(pcb);

	//Aumento el semaforo de cantidad de PCBs en ready 
	sem_post(&pcb_en_ready);
	//log_info(logger, "Envie a memoria los recursos para asignar");
}

void planiCortoPlazo(){
	while (1){
		sem_wait(&pcb_en_ready);
		sem_wait(&CPUVacia);
		//log_info(logger, "Llego pcb a plani corto plazo");
		t_tipo_algoritmo algoritmo;
		algoritmo = obtenerAlgoritmo();

		switch (algoritmo){
		case FIFO:
			log_debug(logger, "Implementando algoritmo FIFO");
			implementar_fifo();

			break;
		case HRRN:
			log_debug(logger, "Implementando algoritmo HRRN");
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

/*
███████╗██╗███████╗░█████╗░
██╔════╝██║██╔════╝██╔══██╗
█████╗░░██║█████╗░░██║░░██║
██╔══╝░░██║██╔══╝░░██║░░██║
██║░░░░░██║██║░░░░░╚█████╔╝
╚═╝░░░░░╚═╝╚═╝░░░░░░╚════╝░
*/

void implementar_fifo(){
	t_pcb *pcb = algoritmo_fifo(LISTA_READY); //Obtiene el 1er elemento de la lista de ready
	//log_info(logger, "Agregando UN pcb a lista exec");
	pasar_a_exec(pcb);
	// Cambio de estado
	//log_info(logger, "Cambio de Estado: PID %d - Estado Anterior: READY , Estado Actual: EXEC", pcb->pid);
	sem_post(&pasar_pcb_a_CPU);
}

t_pcb *algoritmo_fifo(t_list *LISTA_READY){
	t_pcb *pcb = (t_pcb *)list_remove(LISTA_READY, 0);
	return pcb;
}

/*
██╗░░██╗██████╗░██████╗░███╗░░██╗
██║░░██║██╔══██╗██╔══██╗████╗░██║
███████║██████╔╝██████╔╝██╔██╗██║
██╔══██║██╔══██╗██╔══██╗██║╚████║
██║░░██║██║░░██║██║░░██║██║░╚███║
╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚══╝
*/

void implementar_hrrn(){
	t_pcb *pcb = algoritmo_hrrn(LISTA_READY); //Ordena la lista y obtiene el elemento que corresponde
	//log_info(logger, "Agregando UN pcb a lista exec");
	pasar_a_exec(pcb);
	// Cambio de estado
	log_info(logger, "Cambio de Estado: PID %d - Estado Anterior: READY , Estado Actual: EXEC", pcb->pid);
	sem_post(&pasar_pcb_a_CPU);
}

t_pcb *algoritmo_hrrn(t_list *LISTA_READY){
	t_pcb *pcb;
	if (list_size(LISTA_READY) == 1) //Si solo hay uno, lo saco por fifo (el 1ro de la lista)
        pcb = (t_pcb *)list_remove(LISTA_READY, 0);
    else if (list_size(LISTA_READY) > 1){ //Si hay mas tengo que obtener el que tenga el mayor HRRN.
		pcb = list_get_maximum(LISTA_READY, (void*)mayorHRRN);
		list_remove_element(LISTA_READY, pcb); //Y lo tengo que sacar. get_maximum no elimina el elemento de la lista, solo obtiene una copia.
	}
	return pcb;
}

t_pcb* mayorHRRN(t_pcb* unaPCB, t_pcb* otraPCB){
	double unHRRN   = calcular_HRRN(unaPCB);
    double otroHRRN = calcular_HRRN(otraPCB);
    return unHRRN <= otroHRRN
               ? otraPCB //Devuelvo la PCB con el HRRN mayor si se cumple la condicion 
               : unaPCB;
}

double calcular_HRRN(t_pcb* pcb){
	// espera + estimacionCPU
	//------------------------
	//    estimacionCPU
	return (obtener_espera(pcb)+obtener_estimacion(pcb))/(obtener_estimacion(pcb));
}

int obtener_espera(t_pcb* pcb){
	struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
	//                   momento actual               -                  momento de entrada a ready
	return (end.tv_sec * 1000 + end.tv_nsec / 1000000)-(pcb->llegadaReady.tv_sec * 1000 + pcb->llegadaReady.tv_nsec / 1000000);
}

double obtener_estimacion(t_pcb* pcb){
	return (pcb->estimacion_anterior) * (configuracionKernel->HRRN_ALFA) + (pcb->real_anterior) * (1-configuracionKernel->HRRN_ALFA);
}





void iniciar_listas_y_semaforos(){
	LISTA_NEW = list_create();
	LISTA_READY = list_create();
	LISTA_EXEC = list_create();
	LISTA_BLOCKED = list_create();
	LISTA_EXIT  = list_create();

	sem_init(&CantPCBNew, 0, 0);
	sem_init(&CPUVacia, 0, 1);
	sem_init(&pasar_pcb_a_CPU, 0, 0);
	sem_init(&multiprogramacion, 0, configuracionKernel->GRADO_MAX_MULTIPROGRAMACION);	
}

//Supongo que se tiene que liberar en algun momento. Gabi: buena suposición
void liberar_listas_y_semaforos(){}

t_kernel_config *leerConfiguracion(){

	// Creo el config para leer IP y PUERTO
	t_config *config = config_create(CONFIG_PATH);

	// Creo el archivo config
	t_kernel_config *configuracionKernel = malloc(sizeof(t_kernel_config));

	// Leo los datos del config para que kernel funcione como servidor (no tiene IP para funcionar como servido)
	configuracionKernel->PUERTO_ESCUCHA              = config_get_string_value(config, "PUERTO_ESCUCHA");
	configuracionKernel->ALGORITMO_PLANIFICACION     = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configuracionKernel->ESTIMACION_INICIAL          = config_get_int_value(config, "ESTIMACION_INICIAL");
	configuracionKernel->HRRN_ALFA                   = config_get_double_value(config, "HRRN_ALFA");
	configuracionKernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");

	configuracionKernel->RECURSOS                    = config_get_array_value(config, "RECURSOS");
	configuracionKernel->INSTANCIAS_RECURSOS         = config_get_array_value(config, "INSTANCIAS_RECURSOS");

	// Leo los datos del config para que kernel se conecte al resto de modulo
	configuracionKernel->IP_MEMORIA                  = config_get_string_value(config, "IP_MEMORIA");
	configuracionKernel->IP_FILESYSTEM               = config_get_string_value(config, "IP_FILESYSTEM");
	configuracionKernel->IP_CPU                      = config_get_string_value(config, "IP_CPU");

	configuracionKernel->PUERTO_MEMORIA              = config_get_string_value(config, "PUERTO_MEMORIA");
	configuracionKernel->PUERTO_FILESYSTEM           = config_get_string_value(config, "PUERTO_FILESYSTEM");
	configuracionKernel->PUERTO_CPU                  = config_get_string_value(config, "PUERTO_CPU");

	return configuracionKernel;
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/

void cargarRecursos(){
	for (int i = 0; i < string_array_size(configuracionKernel->RECURSOS); i++){
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

/*
███████╗██╗░░░██╗███╗░░██╗░█████╗░██╗░█████╗░███╗░░██╗███████╗░██████╗  ██████╗░███████╗
██╔════╝██║░░░██║████╗░██║██╔══██╗██║██╔══██╗████╗░██║██╔════╝██╔════╝  ██╔══██╗██╔════╝
█████╗░░██║░░░██║██╔██╗██║██║░░╚═╝██║██║░░██║██╔██╗██║█████╗░░╚█████╗░  ██║░░██║█████╗░░
██╔══╝░░██║░░░██║██║╚████║██║░░██╗██║██║░░██║██║╚████║██╔══╝░░░╚═══██╗  ██║░░██║██╔══╝░░
██║░░░░░╚██████╔╝██║░╚███║╚█████╔╝██║╚█████╔╝██║░╚███║███████╗██████╔╝  ██████╔╝███████╗
╚═╝░░░░░░╚═════╝░╚═╝░░╚══╝░╚════╝░╚═╝░╚════╝░╚═╝░░╚══╝╚══════╝╚═════╝░  ╚═════╝░╚══════╝

██████╗░██╗░░░░░░█████╗░███╗░░██╗██╗███████╗██╗░█████╗░░█████╗░░█████╗░██╗░█████╗░███╗░░██╗
██╔══██╗██║░░░░░██╔══██╗████╗░██║██║██╔════╝██║██╔══██╗██╔══██╗██╔══██╗██║██╔══██╗████╗░██║
██████╔╝██║░░░░░███████║██╔██╗██║██║█████╗░░██║██║░░╚═╝███████║██║░░╚═╝██║██║░░██║██╔██╗██║
██╔═══╝░██║░░░░░██╔══██║██║╚████║██║██╔══╝░░██║██║░░██╗██╔══██║██║░░██╗██║██║░░██║██║╚████║
██║░░░░░███████╗██║░░██║██║░╚███║██║██║░░░░░██║╚█████╔╝██║░░██║╚█████╔╝██║╚█████╔╝██║░╚███║
╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═╝░░╚══╝╚═╝╚═╝░░░░░╚═╝░╚════╝░╚═╝░░╚═╝░╚════╝░╚═╝░╚════╝░╚═╝░░╚══╝
*/

void pasar_a_new(t_pcb *pcb){
	pthread_mutex_lock(&listaNew);
	list_add(LISTA_NEW, pcb);
	pthread_mutex_unlock(&listaNew);
	log_debug(logger, "Paso a NEW el proceso %d", pcb->pid);
}

void pasar_a_ready(t_pcb *pcb){
	pthread_mutex_lock(&listaReady);
	list_add(LISTA_READY, pcb);
	struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
	pcb->llegadaReady = start;
	pthread_mutex_unlock(&listaReady);
	log_debug(logger, "Paso a READY el proceso %d", pcb->pid);
}

void pasar_a_exec(t_pcb *pcb){
	pthread_mutex_lock(&listaExec);
	list_add(LISTA_EXEC, pcb);
	pthread_mutex_unlock(&listaExec);
	log_debug(logger, "Paso a EXEC el proceso %d", pcb->pid);
}

void pasar_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&listaExit);
	list_add(LISTA_EXIT, pcb);
	pthread_mutex_unlock(&listaExit);
	log_debug(logger, "Paso a EXIT el proceso %d", pcb->pid);
}


/*
███████╗██╗░░░██╗███╗░░██╗░█████╗░██╗░█████╗░███╗░░██╗███████╗░██████╗  ██████╗░███████╗
██╔════╝██║░░░██║████╗░██║██╔══██╗██║██╔══██╗████╗░██║██╔════╝██╔════╝  ██╔══██╗██╔════╝
█████╗░░██║░░░██║██╔██╗██║██║░░╚═╝██║██║░░██║██╔██╗██║█████╗░░╚█████╗░  ██║░░██║█████╗░░
██╔══╝░░██║░░░██║██║╚████║██║░░██╗██║██║░░██║██║╚████║██╔══╝░░░╚═══██╗  ██║░░██║██╔══╝░░
██║░░░░░╚██████╔╝██║░╚███║╚█████╔╝██║╚█████╔╝██║░╚███║███████╗██████╔╝  ██████╔╝███████╗
╚═╝░░░░░░╚═════╝░╚═╝░░╚══╝░╚════╝░╚═╝░╚════╝░╚═╝░░╚══╝╚══════╝╚═════╝░  ╚═════╝░╚══════╝

███╗░░░███╗███████╗███╗░░██╗░██████╗░█████╗░░░░░░██╗███████╗░██████╗
████╗░████║██╔════╝████╗░██║██╔════╝██╔══██╗░░░░░██║██╔════╝██╔════╝
██╔████╔██║█████╗░░██╔██╗██║╚█████╗░███████║░░░░░██║█████╗░░╚█████╗░
██║╚██╔╝██║██╔══╝░░██║╚████║░╚═══██╗██╔══██║██╗░░██║██╔══╝░░░╚═══██╗
██║░╚═╝░██║███████╗██║░╚███║██████╔╝██║░░██║╚█████╔╝███████╗██████╔╝
╚═╝░░░░░╚═╝╚══════╝╚═╝░░╚══╝╚═════╝░╚═╝░░╚═╝░╚════╝░╚══════╝╚═════╝░
*/

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