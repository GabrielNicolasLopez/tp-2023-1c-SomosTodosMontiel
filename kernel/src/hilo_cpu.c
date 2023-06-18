#include "kernel.h"

void crear_hilo_cpu()
{
	t_razonFinConsola razon;

	// Me conecto a cpu
	conexion_con_cpu = crear_conexion(configuracionKernel->IP_CPU, configuracionKernel->PUERTO_CPU, logger);
	if (conexion_con_cpu == -1) //Si no se puede conectar
	{
		log_error(logger, "KERNEL NO SE CONECTÓ CON CPU. FINALIZANDO KERNEL...");
		exit(1);
	}

	log_debug(logger, "KERNEL SE CONECTO CON CPU...");

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
				break;

			case F_CLOSE:
				break;

			case F_SEEK:
				break;

			case F_READ:
				break;

			case F_WRITE:
				break;

			case F_TRUNCATE:
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
				log_debug(logger, "PID: <%d> - Wait: <%s> - Instancias: <%d>", motivoDevolucion->contextoEjecucion->pid, motivoDevolucion->cadena, recursos_disponibles(motivoDevolucion->cadena));
				//liberar alguna pcb si necesitaba algun recurso que se devolvió
				revisar_recursos_bloqueados(motivoDevolucion->cadena);
				//Reenviamos el contexto			
				se_reenvia_el_contexto = true;
				devolver_ce_a_cpu(motivoDevolucion->contextoEjecucion, conexion_con_cpu);
				break;

			case CREATE_SEGMENT:
				//Enviar a MEMORIA la instruccion de crear segmento y el tamaño
				//Creo un t_segment y asigno id y tamaño
				t_segmento *nuevo_segmento = malloc(sizeof(t_segmento));
				//Cada segmento tiene ID, BASE Y TAMAÑO
				//ID
				nuevo_segmento->id_segmento = motivoDevolucion->cant_int;

				//BASE
				//Creo el paquete y lo envío a memoria con: instruccion, id, tamaño
				crear_segmento(CREATE_SEGMENT, motivoDevolucion->cant_int, motivoDevolucion->cant_intB);
				
				//Esperar y recibir
				recibir_respuesta_create_segment(nuevo_segmento->base); //OK (base del segmento), OUT_OF_MEMORY, COMPACTACION

				//TAMAÑO
				nuevo_segmento->tamanio     = motivoDevolucion->cant_intB;



				switch (respuesta){
				case OK:
					//leer un uint_32 desde el paquete
					//Completo y asigno el segmento a la pcb
					break;
				case OUT_OF_MEMORY:
					se_reenvia_el_contexto = false;
					terminar_consola();
					sem_post(&CPUVacia);
					break;
				case COMPACTACION:
					if(se puede compactar)
						avisar_a_memoria();

					//esperar a terminar
					recibir_tabla_de_segmentos(); //tabla de segmentos actualizada
					actualizar_las_tablas_de_las_pcb();
					crear_segmento(CREATE_SEGMENT, motivoDevolucion->cant_int); //Creo el paquete y lo envío a memoria. instruccion, id, tamaño
					//Devolver el contexto de ejecucion a CPU
					break;
				}

				break;

			/*case DELETE_SEGMENT:

				eliminar_segmento(DELETE_SEGMENT, motivoDevolucion->cant_int); //Creo el paquete y lo envío a memoria. instruccion, id
				respuesta = recibir_respuesta_delete_segment();
				recibir_tabla_de_segmentos(); //tabla de segmentos actualizada

				//Devolver el contexto de ejecucion a CPU

				break;*/		

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

void recibir_respuesta_create_segment(uint32_t *base_segmento){

	t_buffer* respuesta_crear_segmento = buffer_create();
	t_respuestaMemoria respuesta_memoria;

	stream_recv_buffer(conexion_con_memoria, respuesta_crear_segmento);

	//ID del segmento a crear
	buffer_unpack(respuesta_crear_segmento, &respuesta_memoria, sizeof(t_respuestaMemoria));

	switch (respuesta_memoria){
		case OK: //Si puede crear el segmento, tengo que recibir la base del segmento asignado
			buffer_unpack(respuesta_crear_segmento, base_segmento, sizeof(uint32_t));
			break;
		case OUT_OF_MEMORY:
			//Si no hay memoria no se reenviar la PCB. Se la finaliza y libera la CPU ya que no hay memoria.
			se_reenvia_el_contexto = false;
			terminar_consola(OUT_OF_MEMORY);
			sem_post(&CPUVacia);
			break;
		case NECESITO_COMPACTAR:
			//Espero a que sea posible compactar
			sem_wait(&esPosibleCompactar);
				avisar_a_memoria();

	buffer_destroy(respuesta_crear_segmento);
}

void crear_segmento(t_tipoInstruccion instruccion, uint32_t id, uint32_t tamanio){
	
	t_buffer* crear_segmento = buffer_create();

	//ID del segmento a crear
	buffer_pack(crear_segmento, &instruccion, sizeof(t_tipoInstruccion));

	//ID del segmento a crear
	buffer_pack(crear_segmento, &id, sizeof(uint32_t));

	//Tamaño del segmento a crear
	buffer_pack(crear_segmento, &tamanio, sizeof(uint32_t));

	stream_send_buffer(conexion_con_memoria, crear_segmento);

	buffer_destroy(crear_segmento);
}

void eliminar_segmento(t_instruccion instruccion, uint32_t id){
	
	t_buffer* crear_segmento = buffer_create();

	//ID del segmento a crear
	buffer_pack(crear_segmento, &instruccion, sizeof(t_instruccion));

	//ID del segmento a crear
	buffer_pack(crear_segmento, &id, sizeof(uint32_t));

	stream_send_buffer(conexion_con_memoria, crear_segmento);

	buffer_destroy(crear_segmento);
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

void terminar_consola(t_razonFinConsola razon){
	t_pcb *pcb = pcb_ejecutando_remove();

	t_buffer *fin_consola = buffer_create();
	buffer_pack(fin_consola, &razon, sizeof(t_razonFinConsola));
	stream_send_buffer(pcb->contexto->socket, fin_consola);
	buffer_destroy(fin_consola);

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
