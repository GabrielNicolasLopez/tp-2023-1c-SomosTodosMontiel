#include "kernel.h"

int main(void)
{

	// Creo el logger
	logger = log_create(LOG_PATH, MODULE_NAME, 1, LOG_LEVEL_DEBUG);

	log_info(logger, "INICIANDO KERNEL...");

	// Leo la configuracion de kernel
	configuracionKernel = leerConfiguracion();

	// cargarRecursos();

	crear_hilos_kernel();
	log_error(logger, "termine de ejecutar");
}

void crear_hilos_kernel()
{
	pthread_t hiloConsola, hiloCPU, hiloFilesystem, hiloMemoria;

	pthread_create(&hiloConsola, NULL, (void *)crear_hilo_consola, NULL);
	//pthread_create(&hiloCPU, NULL, (void *)crear_hilo_cpu, NULL);
	//pthread_create(&hiloFilesystem, NULL, (void *)crear_hilo_filesystem, NULL);
	//pthread_create(&hiloMemoria, NULL, (void *)crear_hilo_memoria, NULL);

	//pthread_detach(hiloConsola);
	//pthread_detach(hiloCPU);
	//pthread_detach(hiloFilesystem);
	//pthread_detach(hiloMemoria);

	pthread_join(hiloConsola, NULL);
}

void crear_hilo_memoria()
{
	// Me conecto a memoria
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA, logger);
	log_info(logger, "Hola, me conecté a memoria");
	while (1)
	{
	}
}

void crear_hilo_filesystem()
{
	// Me conecto a filesystem
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM, logger);
	log_info(logger, "Hola, me conecté a filesystem");
	while (1)
	{
	}
}

void crear_hilo_cpu()
{
	// Me conecto a cpu
	int conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	log_info(logger, "Hola, me conecté a cpu");
	while (1)
	{
	}
}

void crear_hilo_consola()
{
	int server_fd = iniciar_servidor("127.0.0.1", configuracionKernel->PUERTO_ESCUCHA, logger);
	log_info(logger, "Kernel listo para recibir clientes consola");

	while (1)
	{
		pthread_t hilo_atender_consola;
		int socketCliente = esperar_cliente(server_fd, logger);
		
		int cod_op = recibir_operacion(socketCliente);
		//log_info(logger, "El codigo de operacion es: %s", nombresCodigoOperaciones[cod_op]);

		t_instrucciones instrucciones = recibir_informacion_pfqa(socketCliente);

		//Imprimir instrucciones para ver que se hayan leido bien
		int i=0;
		while(i< instrucciones.cantidadInstrucciones){
			t_instruccion *instruccion = list_get(instrucciones.listaInstrucciones, i);
			log_info(logger, "%s", nombresInstrucciones[instruccion->tipo]);
			i++;
		}

		if(!enviarMensaje(socketCliente, "Llegaron las instrucciones"))
			log_error(logger, "Error al enviar el mensaje");

		crear_pcb(socketCliente, &instrucciones);

		//Envio un paquete para que consola finalice
		t_paquete* paquete = crear_paquete();
		paquete->codigo_operacion = TERMINAR_CONSOLA;

		agregar_a_paquete(paquete, NULL, NULL);

		enviar_paquete(paquete, socketCliente);

		log_info(logger, "Envio de terminar consola");
		
		pthread_create(&hilo_atender_consola, NULL, (void *)crear_pcb, NULL);
		pthread_detach(hilo_atender_consola);
	}
}

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

void crear_pcb(int socketCliente, t_instrucciones *instrucciones)
{
	log_info(logger, "Se conecto una consola");
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->pid = contadorIdPCB;
	pcb->socket = socketCliente;
	pcb-> program_counter= 0;
	pcb-> instrucciones= instrucciones;
	pcb-> tablaDeSegmentos= list_create();
	//pcb-> registrosCPU;
	pcb->estimacionProxRafaga=time(NULL);
	pcb->llegadaReady=time(NULL);
	pcb->taap= list_create();
	list_add(LISTA_NEW, pcb);
	log_debug(logger, "Estado Actual: NEW , proceso id: %d", pcb->pid);
	log_info(logger, "Creación de Proceso: se crea el proceso %d en NEW", pcb->pid); 
	log_info(logger, "Cant de elementos de new: %d", list_size(LISTA_NEW));
}



t_kernel_config *leerConfiguracion()
{

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

t_instrucciones recibir_informacion_pfqa(int cliente_fd){
	int size;
	void *buffer = recibir_buffer(&size, cliente_fd);
	log_info(logger, "size: %d", size);
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
}