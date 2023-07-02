#include "kernel.h"

void hilo_general()
{

	//Me conecto con los módulos
	conectarse_con_cpu();
	//conectarse_con_memoria();
	conectarse_con_fs();

	t_motivoDevolucion *motivoDevolucion = malloc(sizeof(t_motivoDevolucion));
	t_contextoEjecucion *contextoEjecucion = malloc(sizeof(t_contextoEjecucion));
	motivoDevolucion->contextoEjecucion = contextoEjecucion;

	while (1)
	{
		//En caso de que se reenvio un contexto a CPU, no debo esperar la llegada de una nueva PCB porque se 
		//continua utilizando la misma de antes.
		
		//Si no hubo reenvio de contexto.
		if (!se_reenvia_el_contexto)
		{
			//Dedo esperar la llegada de una nueva PCB
			log_debug(logger, "Esperando la llegada de una nueva PCB.");
			// Espero a que un proceso este en la cola de ejecucion
			sem_wait(&pasar_pcb_a_CPU);

			// Obtengo la pcb en ejecucion para crear el contexto de ejecucion
			t_pcb *pcb = pcb_ejecutando();

			// Enviamos el contexto de ejecucion a cpu
			//log_info(logger, "ce enviado a CPU. PID: %d", pcb->contexto->pid);
			enviar_ce_a_cpu(pcb->contexto, conexion_con_cpu);
		}

		log_info(logger, "Esperando cym desde CPU...");
		recibir_cym_desde_cpu(motivoDevolucion, conexion_con_cpu);
		//Al llegar, tenemos que actualizar la PCB con la información que modificó CPU
		actualizar_pcb(motivoDevolucion->contextoEjecucion);

		switch (motivoDevolucion->tipo){
			case IO:
				//Creo el hilo para enviar a la PCB a esperar
				pthread_t hilo_sleep;
				log_debug(logger, "PID: <%d> - Ejecuta IO: <%d>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cant_int);
				//Envio la PCB a la lista de bloqueados
				t_pcb* pcb_a_blocked = pcb_ejecutando_remove();
				log_debug(logger, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", pcb_a_blocked->contexto->pid);
				log_debug(logger, "PID: <%d> - Bloqueado por: <IO>", pcb_a_blocked->contexto->pid);
				pasar_a_blocked(pcb_a_blocked);
				
				//Preparo los datos para enviar la PCB al hilo de sleep
				t_datosIO* datosIO = malloc(sizeof(t_datosIO));
				datosIO->pcb = pcb_a_blocked;
				datosIO->motivo = motivoDevolucion;
				pthread_create(&hilo_sleep, NULL, (void *)sleep_IO, (void *)datosIO);
				// Detacheamos el hilo que es quien se encarga de mandar a ready al pcb
				pthread_detach(hilo_sleep);
				//Como se va a blocked no se reenvia
				se_reenvia_el_contexto = false;
				//Liberamos la CPU
				sem_post(&CPUVacia);
				break;

			case F_OPEN:
			log_debug(logger, "entre a f_open");
				if(existeEnTGAA(motivoDevolucion->cadena)) //Nombre del archivo
				{
					log_debug(logger, "el archivo existe en tgaa");
					agregarArchivoEnTAAP(motivoDevolucion->cadena); //Le pasamos el nombre del archivo
					pasar_a_blocked_de_archivo_de_TGAA(pcb_ejecutando_remove(), motivoDevolucion->cadena); //Le pasamos el nombre del archivo
					//Como se va a blocked no se reenvia
					se_reenvia_el_contexto = false;
					//Liberamos la CPU
					sem_post(&CPUVacia);
				}
				else
				{
					log_debug(logger, "el archivo no existe en tgaa");
					enviar_fopen_a_fs(motivoDevolucion->cadena);
					log_debug(logger, "mensaje enviado a fs, esperando respuesta...");
					sem_wait(&FS_Continue);
					log_debug(logger, "recibi la respuesta de fs, continuando...");
					se_reenvia_el_contexto = true;
					devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
					log_debug(logger, "se devolvio el ce a cpu");
				}
				break;

			case F_CLOSE:
				log_debug(logger, "entre a f_close");
				quitarArchivoEnTAAP(pcb_ejecutando(), motivoDevolucion->cadena);
				if(hayProcesosEsperandoAl(motivoDevolucion->cadena))
				{
					desbloqueo_al_primer_proceso_de_la_cola_del(motivoDevolucion->cadena);
				}else{
					quitarArchivoEnTGAA(motivoDevolucion->cadena);
				}
				//t_pcb* pcb = pcb_ejecutando();
				//log_debug(logger, "archivos en taap de la pcb: %d", list_size(pcb->taap));
				se_reenvia_el_contexto = true;
				devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
				break;

			case F_SEEK:

				actualizar_posicicon_puntero(motivoDevolucion);
				se_reenvia_el_contexto = true;
				devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
				break;

			case F_READ:
				//Disminuyo el semáforo para que se prohiba compactar mientras se ejecuta esta instruccion
				
				pthread_mutex_lock(&mx_instruccion_en_fs);
				instruccion_en_fs++;
				pthread_mutex_unlock(&mx_instruccion_en_fs);

				enviar_fread_a_fs(motivoDevolucion);

				//HACER UN FSEEK ACTUALIZANDO EL PUNTERO LUEGO DE LEER

				//Como la instruccion es bloqueante, bloqueo al proceso en la lista de bloqueados del archivo que está leyendo
				pasar_a_blocked_de_archivo_de_TGAA(pcb_ejecutando_remove(), motivoDevolucion->cadena);
				//No se reenvia porque se fue a blocked
				se_reenvia_el_contexto = false;
				//Liberamos la CPU
				sem_post(&CPUVacia);
				break;

			case F_WRITE:
				//Disminuyo el semáforo para que se prohiba compactar mientras se ejecuta esta instruccion
				
				pthread_mutex_lock(&mx_instruccion_en_fs);
				instruccion_en_fs++;
				pthread_mutex_unlock(&mx_instruccion_en_fs);

				enviar_fwrite_a_fs(motivoDevolucion);

				//HACER UN FSEEK ACTUALIZANDO EL PUNTERO LUEGO DE ESCRIBIR

				//Como la instruccion es bloqueante, bloqueo al proceso en la lista de bloqueados del archivo que está escribiendo
				pasar_a_blocked_de_archivo_de_TGAA(pcb_ejecutando_remove(), motivoDevolucion->cadena);
				//No se reenvia porque se fue a blocked
				se_reenvia_el_contexto = false;
				//Liberamos la CPU
				sem_post(&CPUVacia);
				break;

			case F_TRUNCATE:
				//Disminuyo el semáforo para que se prohiba compactar mientras se ejecuta esta instruccion
				log_debug(logger, "entre a f_truncate");
				pthread_mutex_lock(&mx_instruccion_en_fs);
				instruccion_en_fs++;
				pthread_mutex_unlock(&mx_instruccion_en_fs);

				enviar_ftruncate_a_fs(motivoDevolucion);
				//Como la instruccion es bloqueante, bloqueo al proceso en la lista de bloqueados del archivo que está escribiendo
				pasar_a_blocked_de_archivo_de_TGAA(pcb_ejecutando_remove(), motivoDevolucion->cadena);
				//No se reenvia porque se fue a blocked
				se_reenvia_el_contexto = false;
				//Liberamos la CPU
				sem_post(&CPUVacia);
				break;

			case WAIT:
				// Si no existe el recurso
				if (!existeRecurso(motivoDevolucion->cadena))
				{
					//No se reenvia porque se finaliza la PCB
					se_reenvia_el_contexto = false;
					razon = E_WAIT;
					//Finalizamos la consola con el error
					terminar_consola(razon);
					//Liberamos la CPU
					sem_post(&CPUVacia);
					break;
				}
				// Si hay recursos disponibles (hay mas de 0)
				if (recursos_disponibles(motivoDevolucion->cadena) > 0)
				{
					// Le asignamos a la PCB ejecutando el recurso pedido
					asignarRecurso(motivoDevolucion->cadena);
					//log_info(logger, "PID %d robó un recurso: %s", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cadena);
					log_debug(logger, "PID: <%d> - Wait: <%s> - Instancias: <%d>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cadena, recursos_disponibles(motivoDevolucion->cadena));
					//Renviamos el contexto porque la solicitud fue exitosa
					se_reenvia_el_contexto = true;
					devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
					break;
				}
				// Si el recurso existe pero no hay instancias disponibles en este momento, se envia a la lista de bloqueo
				else if (recursos_disponibles(motivoDevolucion->cadena) <= 0)
				{
					// Le pasamos la pcb y el nombre del recurso para que lo bloquee
					pasar_a_blocked_de_recurso(pcb_ejecutando_remove(), motivoDevolucion->cadena);
					//No se reenvia porque se fue a blocked
					se_reenvia_el_contexto = false;
					//Liberamos la CPU
					sem_post(&CPUVacia);
				}
				break;
			
			case SIGNAL:
				// Si no existe el recurso
				if (!existeRecurso(motivoDevolucion->cadena))
				{
					//No se reenvia porque finalizamos la PCB
					se_reenvia_el_contexto = false;
					razon = E_SIGNAL;
					terminar_consola(razon);
					//Liberamos la PCB
					sem_post(&CPUVacia);
					break;
				}
				// Si existe el recurso
				devolverRecurso(motivoDevolucion->cadena);
				log_debug(logger, "PID: <%d> - Signal: <%s> - Instancias: <%d>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cadena, recursos_disponibles(motivoDevolucion->cadena));
				//liberar alguna pcb si necesitaba algun recurso que se devolvió
				actualizar_procesos_bloqueados(motivoDevolucion->cadena);
				//Reenviamos el contexto			
				se_reenvia_el_contexto = true;
				devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
				break;

			/*case CREATE_SEGMENT:
				
				log_debug(logger, "PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cant_int, motivoDevolucion->cant_intB);
				//Enviar a MEMORIA la instruccion de crear segmento y el tamaño
				//Creo un t_segment y asigno id y tamaño
				t_segmento *nuevo_segmento = malloc(sizeof(t_segmento));
				
				//Cada segmento tiene ID, BASE Y TAMAÑO
				
				//ID - El ID del segmento viene dado por parámetro
				nuevo_segmento->id_segmento = motivoDevolucion->cant_int;
				
				//BASE - La base del segmento se la pregunto a memoria
				//Creo el paquete y se lo envío a memoria con: instruccion, id, tamaño
				crear_segmento(motivoDevolucion->cant_int, motivoDevolucion->cant_intB);
				
				//Espero la respuesta de memoria y pueden pasar 3 cosas: OK (base del segmento), OUT_OF_MEMORY, COMPACTACION
				recibir_respuesta_create_segment(nuevo_segmento->base, motivoDevolucion->cant_int, motivoDevolucion->cant_intB);

				//TAMAÑO
				nuevo_segmento->tamanio = motivoDevolucion->cant_intB;

				agregar_segmento(pcb_ejecutando(), nuevo_segmento);
				break;

			case DELETE_SEGMENT:
				log_debug(logger, "PID: <PID> - Eliminar Segmento - Id Segmento: <ID SEGMENTO>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cant_int);

				eliminar_segmento(motivoDevolucion->cant_int); //Creo el paquete y lo envío a memoria. instruccion, id
				respuesta = recibir_respuesta_delete_segment();
				recibir_tabla_de_segmentos(); //tabla de segmentos actualizada

				//Devolver el contexto de ejecucion a CPU
				break;	*/

			case YIELD:
				//Sacamos a la PCB de ejecutando
				t_pcb* pcb_a_ready = pcb_ejecutando_remove();
				log_debug(logger, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <READY>", pcb_a_ready->contexto->pid);
				//La mandamos a ready
				pasar_a_ready(pcb_a_ready);
				//No se reenvia el contexto porque se fue a ready
				se_reenvia_el_contexto = false;
				//Liberamos la CPU
				sem_post(&CPUVacia);
				break;
			
			case EXIT:
				se_reenvia_el_contexto = false;
				razon = FIN;
				terminar_consola(razon);
				sem_post(&CPUVacia);
				break;

			default:
				log_error(logger, "No se reconoce la instruccion recibida de CPU...");
				break;
			}
	}
}

void actualizar_posicicon_puntero(t_motivoDevolucion *motivoDevolucion){
	//Busco la entrada en la TGAA
	t_entradaTGAA *entradaTGAA = devolverEntradaTGAA(motivoDevolucion->cadena);
	//Modifico el valor del puntero
	entradaTGAA->posicionPuntero = motivoDevolucion->cant_int;
}



void conectarse_con_cpu(){
	// Me conecto a cpu
	conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU);
	if (conexion_con_cpu == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON CPU. FINALIZANDO KERNEL...");
		//kernel_destroy(configuracionKernel, logger);
		exit(-1);
	}

	stream_send_empty_buffer(conexion_con_cpu, (uint8_t) HANDSHAKE_kernel);
    uint8_t respuesta_cpu = stream_recv_header(conexion_con_cpu);
	stream_recv_empty_buffer(conexion_con_cpu);
	log_info(logger, "handshake HANDSHAKE_ok_continue = 4: %d", respuesta_cpu);

    if (respuesta_cpu != HANDSHAKE_ok_continue)
	{
        log_error(logger, "Error al hacer handshake con módulo Cpu");
        //kernel_destroy(configuracionKernel, logger);
        exit(-1);
    }

	log_debug(logger, "KERNEL SE CONECTO CON CPU.");
}
void conectarse_con_memoria(){
	// Conexión con Memoria
    conexion_con_memoria = crear_conexion(configuracionKernel->IP_MEMORIA, configuracionKernel->PUERTO_MEMORIA);
    if (conexion_con_memoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando KERNEL...");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }

    stream_send_empty_buffer(conexion_con_memoria, HANDSHAKE_kernel);
    uint8_t memoriaResponse = stream_recv_header(conexion_con_memoria);
    stream_recv_empty_buffer(conexion_con_memoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        //kernel_destroy(configuracionKernel);
        exit(-1);
    }
    log_info(logger, "KERNEL SE CONECTO CON MEMORIA.");

}
void conectarse_con_fs(){
	// Me conecto a filesystem
	conexion_con_fs = crear_conexion(configuracionKernel->IP_FILESYSTEM, configuracionKernel->PUERTO_FILESYSTEM);
	if (conexion_con_fs == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON FS. FINALIZANDO KERNEL...");
		//kernel_destroy(configuracionKernel, logger);
		exit(-1);
	}

	log_debug(logger, "Enviando ");
	stream_send_empty_buffer(conexion_con_fs, (uint8_t) HANDSHAKE_kernel);
    uint8_t fsResponse = stream_recv_header(conexion_con_fs);
	stream_recv_empty_buffer(conexion_con_fs);

    if (fsResponse != HANDSHAKE_ok_continue)
	{
        log_error(logger, "Error al hacer handshake con módulo FS");
        //kernel_destroy(configuracionKernel, logger);
        exit(-1);
    }
	
	log_debug(logger, "KERNEL SE CONECTO CON FS.");

	pthread_t hilo_respuestas_fs;
	pthread_create(&hilo_respuestas_fs, NULL, (void *) esperar_respuestas, NULL);
	pthread_detach(hilo_respuestas_fs);
}

void esperar_respuestas(){

	bool compacto;

	while(1){
		
		log_debug(logger, "KERNEL ESPERANDO RESPUESTAS DE FS...");

		t_FS_header header = stream_recv_header(conexion_con_fs);
		//stream_recv_empty_buffer(conexion_con_fs);

		char* nombreArchivo = recibir_nombre_de_archivo_de_fs();
		log_error(logger, "nombre del archivo recibido: %s", nombreArchivo);

		switch (header)
		{
		//INSTRUCCIONES BLOQUEANTES FREAD, FWRITE, FTRUNCATE SE MANEJAN CON FS_OK + NOMBRE ARCHIVO
		//CUANDO LLEGA EL FS_OK + ARCHIVO, SACAMOS AL PROCESO DE LA LISTA DE BLOCKED
		case FS_OK:
			log_error(logger, "entro a desbloquear una pcb");
			//Recibimos el nombre del archivo y sacamos al proceso de la lista de blocked
			t_pcb* pcb_a_ready = sacar_de_blocked_de_archivo_de_TGAA(nombreArchivo);
			pthread_mutex_lock(&mx_instruccion_en_fs);
			instruccion_en_fs--;
			pthread_mutex_unlock(&mx_instruccion_en_fs);
			pthread_mutex_lock(&mx_hayQueCompactar);
			compacto = hayQueCompactar;
			pthread_mutex_unlock(&mx_hayQueCompactar);
			if(compacto){
				sem_post(&espera_instrucciones);
			}
			log_debug(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb_a_ready->contexto->pid);
			//Lo pasamos a ready
			pasar_a_ready(pcb_a_ready);
			break;
		
		case FS_CREATE_OK:
		case FS_OPEN_OK:
			log_debug(logger, "el archivo ya está abierto, continuando...");
			//Si el archivo se pudo abrir correctamente
			agregarArchivoEnTGAA(nombreArchivo); //Revisar
			agregarArchivoEnTAAP(nombreArchivo); //Revisar
			sem_post(&FS_Continue);
			break;

		case FS_OPEN_NO_OK:
			//Si no está abierto lo tenemos que crear
			log_error(logger, "reenvio el nombre del archivo como fcreate a fs: %s", nombreArchivo);
			enviar_fcreate_a_fs(nombreArchivo);
			break;

		default:
			log_error(logger, "FS ENVIO UN HEADER CUALQUIERAAAA");
			break;
		}

		
	}
}

// t_pcb* sacar_de_blocked_de_archivo_de_TGAA(char* nombre_archivo)
// {
// 	t_pcb* pcb_a_ready;

// 	int posicion;
// 	t_entradaTGAA *entradaTGAA;
// 	for (int i = 0; i < list_size(LISTA_TGAA); i++)
// 	{
// 		entradaTGAA = list_get(LISTA_TGAA, posicion);
// 		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
// 			posicion = i;
// 	}

// 	//Obtengo la entrada
// 	entradaTGAA = list_get(LISTA_TGAA, posicion);	
// 	//Saco la pcb de blocked
// 	pthread_mutex_lock(&entradaTGAA->mutex_lista_block_archivo);
// 	pcb_a_ready = list_remove(entradaTGAA->lista_block_archivo, 0); //Saco la pcb que estaba bloqueada
// 	pthread_mutex_unlock(&entradaTGAA->mutex_lista_block_archivo);

// 	return pcb_a_ready;
// }

t_pcb* sacar_de_blocked_de_archivo_de_TGAA(char* nombre_archivo)
{
	t_pcb* pcb_a_ready;

	int posicion;
	t_entradaTGAA *entradaTGAA;
	for (int i = 0; i < list_size(LISTA_TGAA); i++)
	{
		entradaTGAA = list_get(LISTA_TGAA, i);
		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			break;
	}

	//Obtengo la entrada
	//entradaTGAA = list_get(LISTA_TGAA, posicion);	
	//Saco la pcb de blocked
	pthread_mutex_lock(&entradaTGAA->mutex_lista_block_archivo);
	pcb_a_ready = list_remove(entradaTGAA->lista_block_archivo, 0); //Saco la pcb que estaba bloqueada
	pthread_mutex_unlock(&entradaTGAA->mutex_lista_block_archivo);

	return pcb_a_ready;
}

char* recibir_nombre_de_archivo_de_fs()
{
	t_buffer* buffer_respuesta = buffer_create();
	int longitud_nombreArchivo;

	stream_recv_buffer(conexion_con_fs, buffer_respuesta);

	//Longitud cadena
	buffer_unpack(buffer_respuesta, &longitud_nombreArchivo, sizeof(uint32_t));
	char* nombreArchivo = malloc(longitud_nombreArchivo);
	
	//Cadena
	buffer_unpack(buffer_respuesta, nombreArchivo, longitud_nombreArchivo);
	//log_debug(logger, "El nombre de archivo recibido de FS es: %s", nombreArchivo);

	buffer_destroy(buffer_respuesta);
	return nombreArchivo;
}

void enviar_ftruncate_a_fs(t_motivoDevolucion *motivoDevolucion){
	
	t_buffer* buffer_ftruncate = buffer_create();

	//Tipo de instruccion
	buffer_pack(buffer_ftruncate, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_ftruncate, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_ftruncate, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	//Tamaño del archivo
	buffer_pack(buffer_ftruncate, &motivoDevolucion->cant_int, sizeof(uint32_t));
	
	stream_send_buffer(conexion_con_fs, 0, buffer_ftruncate);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_ftruncate->size);

	buffer_destroy(buffer_ftruncate);
}

void enviar_fcreate_a_fs(char *nombreArchivo)
{
	
	t_buffer* buffer_fcreate = buffer_create();

	t_tipoInstruccion tipoInstruccion = F_CREATE;
	uint32_t longitudCadena = string_length(nombreArchivo)+1;

	//Tipo de instruccion
	buffer_pack(buffer_fcreate, &tipoInstruccion, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_fcreate, &longitudCadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_fcreate, nombreArchivo, longitudCadena);
	
	stream_send_buffer(conexion_con_fs, (uint8_t) FS_INSTRUCCION, buffer_fcreate);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_fcreate->size);

	buffer_destroy(buffer_fcreate);
}

void enviar_fwrite_a_fs(t_motivoDevolucion *motivoDevolucion){
	
	t_buffer* buffer_fwrite = buffer_create();

	//Tipo de instruccion
	buffer_pack(buffer_fwrite, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_fwrite, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_fwrite, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	//Dirección Lógica
	buffer_pack(buffer_fwrite, &motivoDevolucion->cant_int, sizeof(uint32_t));
	//Cantidad de Bytes
	buffer_pack(buffer_fwrite, &motivoDevolucion->cant_intB, sizeof(uint32_t));

	stream_send_buffer(conexion_con_fs, 0, buffer_fwrite);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_fwrite->size);

	buffer_destroy(buffer_fwrite);
}

void enviar_fread_a_fs(t_motivoDevolucion *motivoDevolucion){
	t_buffer* buffer_fread = buffer_create();

	//Tipo de instruccion
	buffer_pack(buffer_fread, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_fread, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_fread, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	//Dirección Lógica
	buffer_pack(buffer_fread, &motivoDevolucion->cant_int, sizeof(uint32_t));
	//Cantidad de Bytes
	buffer_pack(buffer_fread, &motivoDevolucion->cant_intB, sizeof(uint32_t));

	stream_send_buffer(conexion_con_fs, 0, buffer_fread);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_fread->size);

	buffer_destroy(buffer_fread);
}

void enviar_fseek_a_fs(t_motivoDevolucion *motivoDevolucion){ 			//	F_SEEK (Nombre Archivo, Posición):
	
	t_buffer* buffer_fseek = buffer_create();

	//Tipo de instruccion
	buffer_pack(buffer_fseek, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_fseek, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_fseek, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	//Posicion del fseek
	buffer_pack(buffer_fseek, &motivoDevolucion->cant_int, sizeof(uint32_t));

	stream_send_buffer(conexion_con_fs, 0, buffer_fseek);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_fseek->size);

	buffer_destroy(buffer_fseek);
}

void enviar_fopen_a_fs(char *nombreArchivo){
	
	t_buffer* buffer_fopen = buffer_create();

	t_tipoInstruccion tipoInstruccion = F_OPEN;
	uint32_t longitudCadena = string_length(nombreArchivo)+1;

	//Tipo de instruccion
	buffer_pack(buffer_fopen, &tipoInstruccion, sizeof(t_tipoInstruccion));
	//Tamaño de la cadena
	buffer_pack(buffer_fopen, &longitudCadena, sizeof(uint32_t));
    //Cadena
    buffer_pack(buffer_fopen, nombreArchivo, longitudCadena);
	
	stream_send_buffer(conexion_con_fs, (uint8_t) FS_INSTRUCCION, buffer_fopen);
	log_error(logger, "Tamaño de la instruccion enviada a FS %d", buffer_fopen->size);

	buffer_destroy(buffer_fopen);
}

void recibir_respuesta_create_segment(uint32_t base_segmento, uint32_t id, uint32_t tamanio){

	t_buffer* respuesta_crear_segmento = buffer_create();
	
	//int valor_esPosibleCompactar;

	t_Kernel_Memoria respuesta_memoria = stream_recv_header(conexion_con_memoria);

	stream_recv_buffer(conexion_con_memoria, respuesta_crear_segmento);

	switch (respuesta_memoria){
		case BASE: //Si puede crear el segmento, tengo que recibir la base del segmento asignado
			buffer_unpack(respuesta_crear_segmento, &base_segmento, sizeof(uint32_t));
			break;
		case SIN_MEMORIA:
			//Si no hay memoria no se reenvia la PCB. Se la finaliza y libera la CPU ya que no hay memoria.
			se_reenvia_el_contexto = false;
			terminar_consola(OUT_OF_MEMORY);
			sem_post(&CPUVacia);
			break;
		case NECESITO_COMPACTAR:
			//Espero a que sea posible compactar
			log_debug(logger, "Kernel está esperando para poder compactar");
			sem_wait(&esPosibleCompactar);
			esperandoParaCompactar();
			pedir_a_memoria_que_compacte();
			log_debug(logger, "Compactación: <Se solicitó compactación / Esperando Fin de Operaciones de FS>");
			esperar_respuesta_compactacion(CREATE_SEGMENT, id, tamanio);
			log_debug(logger, "Se finalizó el proceso de compactación");
			crear_segmento(id, tamanio);
			recibir_respuesta_create_segment(base_segmento, -1, -1);
		default:
			log_error(logger, "Mensaje de memoria no valido en la creacion de un segmento");
			break;
	}

	buffer_destroy(respuesta_crear_segmento);
}

void esperandoParaCompactar(){

	if(instruccion_en_fs > 0){ 
		pthread_mutex_lock(&mx_hayQueCompactar);
		hayQueCompactar = true;
		pthread_mutex_lock(&mx_instruccion_en_fs);
		sem_init(&espera_instrucciones, 0, -instruccion_en_fs+1);
		pthread_mutex_unlock(&mx_instruccion_en_fs);
		pthread_mutex_unlock(&mx_hayQueCompactar);
		sem_wait(&espera_instrucciones);
		sem_destroy(&espera_instrucciones);
	}
	pthread_mutex_lock(&mx_hayQueCompactar);
	hayQueCompactar = false;
	pthread_mutex_unlock(&mx_hayQueCompactar);
}

void esperar_respuesta_compactacion(){

	t_Kernel_Memoria respuesta_compactacion = stream_recv_header(conexion_con_memoria);

	if(respuesta_compactacion != FIN_COMPACTACION)
		log_error(logger, "Memoria no compactó correctamente");

}

void pedir_a_memoria_que_compacte(){
	
	// t_buffer* empezar_compactacion = buffer_create();
	// t_Kernel_Memoria mensaje = EMPEZA_A_COMPACTAR;

	//Peticion a memoria
	// buffer_pack(empezar_compactacion, &mensaje, sizeof(t_Kernel_Memoria));

	// stream_send_buffer(conexion_con_memoria, EMPEZA_A_COMPACTAR, empezar_compactacion);

	//buffer_destroy(empezar_compactacion);

	stream_send_empty_buffer(conexion_con_memoria, EMPEZA_A_COMPACTAR);
}

void crear_segmento(uint32_t id, uint32_t tamanio){
	
	t_buffer* crear_segmento = buffer_create();
	t_tipoInstruccion instruccion = CREATE_SEGMENT;

	//Tipo de instruccion: crear/borrar
	buffer_pack(crear_segmento, &instruccion, sizeof(t_tipoInstruccion));

	//ID del segmento a crear
	buffer_pack(crear_segmento, &id, sizeof(uint32_t));

	//Tamaño del segmento a crear
	buffer_pack(crear_segmento, &tamanio, sizeof(uint32_t));

	stream_send_buffer(conexion_con_memoria, INSTRUCCION, crear_segmento);

	buffer_destroy(crear_segmento);
}

void eliminar_segmento(uint32_t id){
	
	t_buffer* eliminar_segmento = buffer_create();
	t_tipoInstruccion instruccion = DELETE_SEGMENT;

	//Tipo de instruccion: crear/borrar
	buffer_pack(eliminar_segmento, &instruccion, sizeof(t_tipoInstruccion));

	//ID del segmento a borrar
	buffer_pack(eliminar_segmento, &id, sizeof(uint32_t));

	stream_send_buffer(conexion_con_memoria, INSTRUCCION, eliminar_segmento);

	buffer_destroy(eliminar_segmento);
}

void recibir_respuesta_delete_segment(){

	t_buffer* respuesta_eliminar_segmento = buffer_create();
	//t_tipoInstruccion instruccion = DELETE_SEGMENT;

	stream_recv_buffer(conexion_con_memoria, respuesta_eliminar_segmento);

	//Tipo de instruccion: crear/borrar
	//buffer_unpack(eliminar_segmento, &instruccion, sizeof(t_tipoInstruccion));


	buffer_destroy(respuesta_eliminar_segmento);

}

void actualizar_pcb(t_contextoEjecucion *contextoEjecucion)
{
	t_pcb *pcb = pcb_ejecutando();
	pcb->contexto = contextoEjecucion;
}

void devolver_ce_a_cpu(t_contextoEjecucion *contextoEjecucion, int conexion_con_cpu)
{
	log_info(logger, "ce enviado a CPU. PID: %d", contextoEjecucion->pid);
	enviar_ce_a_cpu(contextoEjecucion, conexion_con_cpu);
}

void sleep_IO(t_datosIO *datosIO){
	int tiempo = datosIO->motivo->cant_int;
	sleep(tiempo);
	list_remove_element(LISTA_BLOCKED, datosIO->pcb);
	log_debug(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", datosIO->pcb->contexto->pid);
	pasar_a_ready(datosIO->pcb);
}

void terminar_consola(t_Kernel_Consola razon){
	t_pcb *pcb = pcb_ejecutando_remove();

	stream_send_empty_buffer(pcb->contexto->socket, razon);

	log_error(logger, "Finaliza el proceso <%d> - Motivo: <%s>", pcb->contexto->pid, razonFinConsola[razon]);
	
	sem_post(&multiprogramacion);
}

t_pcb *pcb_ejecutando(){
	//log_debug(logger, "Obteniendo pcb ejecutando");
	pthread_mutex_lock(&listaExec);
	t_pcb *pcb = list_get(LISTA_EXEC, 0);
	pthread_mutex_unlock(&listaExec);
	return pcb;
}

t_pcb *pcb_ejecutando_remove(){
	//log_debug(logger, "Obteniendo y eliminando pcb ejecutando");
	pthread_mutex_lock(&listaExec);
	t_pcb *pcb = list_get(LISTA_EXEC, 0); // Primero obtengo una copia de la pcb
	list_remove(LISTA_EXEC, 0);			  // Elimino la pcb de la lista
	pthread_mutex_unlock(&listaExec);
	return pcb;
}

bool existeEnTGAA(char *nombre_archivo)
{
	for (int i = 0; i < list_size(LISTA_TGAA); i++)
	{
		t_entradaTGAA *entradaTGAA = list_get(LISTA_TGAA, i);
		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			return true;
	}
	return false;
}    

// t_entradaTGAA* devolverEntradaTGAA(char *nombre_archivo)
// {
// 	int posicion;
// 	t_entradaTGAA *entradaTGAA = malloc(sizeof(t_entradaTGAA));

// 	log_error(logger, "cantidad de archivos en TGAA (al devolver): %d", list_size(LISTA_TGAA));
	
// 	for (int i = 0; i < list_size(LISTA_TGAA); i++)
// 	{
// 		entradaTGAA = list_get(LISTA_TGAA, posicion);
// 		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
// 			posicion = i;
// 	}

// 	entradaTGAA = list_get(LISTA_TGAA, posicion);
// 	return entradaTGAA;
// }

t_entradaTGAA* devolverEntradaTGAA(char *nombre_archivo)
{
	t_entradaTGAA *entradaTGAA;// = malloc(sizeof(t_entradaTGAA));

	log_error(logger, "cantidad de archivos en TGAA (al devolver): %d", list_size(LISTA_TGAA));
	
	for (int i = 0; i < list_size(LISTA_TGAA); i++)
	{
		entradaTGAA = list_get(LISTA_TGAA, i);
		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			return entradaTGAA;
	}
}

void agregarEnTGAA(t_entradaTGAA *entradaTGAA)
{
	pthread_mutex_lock(&listaTGAA);
	list_add(LISTA_TGAA, entradaTGAA);
	pthread_mutex_unlock(&listaTGAA);
	log_error(logger, "cantidad de archivos en TGAA: %d", list_size(LISTA_TGAA));
}

void recibir_respuesta_fopen_desde_fs(t_FS_header* respuesta_fs)
{
	t_buffer* buffer_respuesta_fs = buffer_create();
	*respuesta_fs = stream_recv_header(conexion_con_fs);
	stream_recv_buffer(conexion_con_memoria, buffer_respuesta_fs);
	buffer_destroy(buffer_respuesta_fs);
}

// void agregarArchivoEnTAAP(char *nombreArchivo){
// 	t_pcb *pcb = pcb_ejecutando();
// 	t_entradaTGAA *entradaTGAA = malloc(sizeof(t_entradaTGAA));
// 	entradaTGAA = devolverEntradaTGAA(nombreArchivo);
// 	//t_entradaTGAA *entradaTGAA = devolverEntradaTGAA(nombreArchivo);
// 	pthread_mutex_lock(&pcb->mutex_TAAP);
// 	list_add(pcb->taap, entradaTGAA);
// 	pthread_mutex_unlock(&pcb->mutex_TAAP);
// 	log_error(logger, "cantidad de archivos en TGAA: %d", list_size(LISTA_TGAA));
// }

void agregarArchivoEnTAAP(char *nombreArchivo){
	t_pcb *pcb = pcb_ejecutando();
	t_entradaTAAP *entradaTAAP = malloc(sizeof(t_entradaTAAP));
	entradaTAAP->entradaTGAA = devolverEntradaTGAA(nombreArchivo);
	//t_entradaTGAA *entradaTGAA = devolverEntradaTGAA(nombreArchivo);
	pthread_mutex_lock(&pcb->mutex_TAAP);
	list_add(pcb->taap, entradaTAAP);
	pthread_mutex_unlock(&pcb->mutex_TAAP);
	log_error(logger, "PID: %d. cantidad de archivos en pcb->taap: %d", pcb->contexto->pid, list_size(pcb->taap));
}

void agregarArchivoEnTGAA(char* nombreArchivo)
{	
	t_entradaTGAA *entradaTGAA = malloc(sizeof(t_entradaTGAA));
	entradaTGAA->nombreArchivo = malloc(string_length(nombreArchivo)+1);

	entradaTGAA->nombreArchivo = nombreArchivo;
	entradaTGAA->posicionPuntero = 0;
	entradaTGAA->tamanioArchivo = 0;
	entradaTGAA->lista_block_archivo = list_create();
	pthread_mutex_init(&entradaTGAA->mutex_lista_block_archivo, NULL);
	agregarEnTGAA(entradaTGAA);
}

// void pasar_a_blocked_de_archivo_de_TGAA(t_pcb *pcb_a_blocked, char* nombre_archivo)
// {
// 	int posicion;
// 	t_entradaTGAA *entradaTGAA;
// 	for (int i = 0; i < list_size(LISTA_TGAA); i++)
// 	{
// 		entradaTGAA = list_get(LISTA_TGAA, posicion);
// 		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
// 			posicion = i;
// 	}

// 	entradaTGAA = list_get(LISTA_TGAA, posicion);
// 	log_debug(logger, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", pcb_a_blocked->contexto->pid);
// 	log_debug(logger, "PID: <%d> - Bloqueado por: <%s>", pcb_a_blocked->contexto->pid, nombre_archivo);
// 	pthread_mutex_lock(&entradaTGAA->mutex_lista_block_archivo);
// 	list_add(entradaTGAA->lista_block_archivo, pcb_a_blocked);
// 	pthread_mutex_unlock(&entradaTGAA->mutex_lista_block_archivo);
// }

void pasar_a_blocked_de_archivo_de_TGAA(t_pcb *pcb_a_blocked, char* nombre_archivo)
{
	t_entradaTGAA *entradaTGAA;
	for (int i = 0; i < list_size(LISTA_TGAA); i++)
	{
		entradaTGAA = list_get(LISTA_TGAA, i);
		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			break;
	}

	//entradaTGAA = list_get(LISTA_TGAA, posicion);
	log_debug(logger, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", pcb_a_blocked->contexto->pid);
	log_debug(logger, "PID: <%d> - Bloqueado por: <%s>", pcb_a_blocked->contexto->pid, nombre_archivo);
	pthread_mutex_lock(&entradaTGAA->mutex_lista_block_archivo);
	list_add(entradaTGAA->lista_block_archivo, pcb_a_blocked);
	pthread_mutex_unlock(&entradaTGAA->mutex_lista_block_archivo);
}

// void quitarArchivoEnTAAP(t_pcb *pcb, char *nombre_archivo)
// {
// 	int posicion;
// 	t_entradaTAAP *entradaTAAP;
// 	for (int i = 0; i < list_size(pcb->taap); i++)
// 	{
// 		entradaTAAP = list_get(pcb->taap, i);
// 		if (strcmp(entradaTAAP->entradaTGAA->nombreArchivo, nombre_archivo) == 0)
// 			posicion = i;
// 	}

// 	pthread_mutex_lock(&pcb->mutex_TAAP);
// 	entradaTAAP = list_remove(pcb->taap, posicion); // Elimino el archivo de la lista taap
// 	pthread_mutex_unlock(&pcb->mutex_TAAP);
// 	free(entradaTAAP); //recordatorio: no hacer free al puntero *entradaTGAA
// }

void quitarArchivoEnTAAP(t_pcb *pcb, char *nombre_archivo)
{
	int posicion;
	t_entradaTAAP *entradaTAAP = malloc(sizeof(t_entradaTAAP));
	entradaTAAP->entradaTGAA = malloc(sizeof(t_entradaTGAA));
	//t_entradaTAAP *entradaTAAP;

	log_error(logger, "cantidad de archivos en TAAP: %d", list_size(pcb->taap));
	for (int i = 0; i < list_size(pcb->taap); i++)
	{
		entradaTAAP = list_get(pcb->taap, i);
		if (strcmp(entradaTAAP->entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			posicion = i;
	}

	pthread_mutex_lock(&pcb->mutex_TAAP);
	entradaTAAP = list_remove(pcb->taap, posicion); // Elimino el archivo de la lista taap
	pthread_mutex_unlock(&pcb->mutex_TAAP);
	free(entradaTAAP); //recordatorio: no hacer free al puntero *entradaTGAA
}

bool hayProcesosEsperandoAl(char *nombre_archivo)
{
	t_entradaTGAA* entradaTGAA = devolverEntradaTGAA(nombre_archivo);
	if(list_size(entradaTGAA->lista_block_archivo) > 0)
		return true;
	return false;
}

void desbloqueo_al_primer_proceso_de_la_cola_del(char *nombre_archivo)
{
	t_entradaTGAA* entradaTGAA = devolverEntradaTGAA(nombre_archivo);
	t_pcb *pcb_blocked_a_ready = list_remove(entradaTGAA->lista_block_archivo, 0);
	//Y lo mandamos a la cola de ready
	log_debug(logger, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", pcb_blocked_a_ready->contexto->pid);
	pasar_a_ready(pcb_blocked_a_ready);
}

void quitarArchivoEnTGAA(char *nombre_archivo)
{
	int posicion;
	t_entradaTGAA* entradaTGAA;
	for (int i = 0; i < list_size(LISTA_TGAA); i++)
	{
		entradaTGAA = list_get(LISTA_TGAA, i);
		if (strcmp(entradaTGAA->nombreArchivo, nombre_archivo) == 0)
			posicion = i;
	}
	entradaTGAA = list_remove(LISTA_TGAA, posicion);
	free(entradaTGAA->nombreArchivo);
	list_destroy(entradaTGAA->lista_block_archivo);
	pthread_mutex_destroy(&entradaTGAA->mutex_lista_block_archivo);
	free(entradaTGAA);
}

