#include "kernel.h"

void enviar_ce_a_cpu(t_pcb *pcb, int conexion_con_cpu)
{

	t_buffer *ce_a_enviar = buffer_create();

	// Socket
	buffer_pack(ce_a_enviar, &pcb->contexto->socket, sizeof(uint32_t));
    //log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	// PID
	buffer_pack(ce_a_enviar, &pcb->contexto->pid, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	// PC
	buffer_pack(ce_a_enviar, &pcb->contexto->program_counter, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

	// Instrucciones
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	for (int i = 0; i < pcb->contexto->instrucciones->cantidadInstrucciones; i++)
	{
		instruccion = list_get(pcb->contexto->instrucciones->listaInstrucciones, i);
		// log_debug(logger, "obtuve la instruccion: %d", i+1);
		buffer_pack(ce_a_enviar, &(instruccion->tipo), sizeof(t_tipoInstruccion));
		//log_error(logger, "TAMAÑO DEL BUFFER I%d", ce_a_enviar->size);

		if (instruccion->tipo == SET)
		{
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == MOV_IN)
		{
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == MOV_OUT)
		{
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->registro, sizeof(t_registro));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == IO)
		{
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_OPEN)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_CLOSE)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_SEEK)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_READ)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntB, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_WRITE)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntB, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == F_TRUNCATE)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == WAIT)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == SIGNAL)
		{
			buffer_pack(ce_a_enviar, &instruccion->longitud_cadena, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, instruccion->cadena, instruccion->longitud_cadena);
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == CREATE_SEGMENT)
		{
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			buffer_pack(ce_a_enviar, &instruccion->paramIntB, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		else if (instruccion->tipo == DELETE_SEGMENT)
		{
			buffer_pack(ce_a_enviar, &instruccion->paramIntA, sizeof(uint32_t));
			// log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
		}
		//Para los casos EXIT y YIELD no se envían mas parámetros y por eso no existe
		//un if para dichos casos
	}

	//Registros C, E y R
	buffer_pack(ce_a_enviar, pcb->contexto->registrosCPU->registroC, sizeof(t_registroC));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	buffer_pack(ce_a_enviar, pcb->contexto->registrosCPU->registroE, sizeof(t_registroE));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	buffer_pack(ce_a_enviar, pcb->contexto->registrosCPU->registroR, sizeof(t_registroR));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

	buffer_pack(buffer, &(pcb->tamanio_tabla), sizeof(uint32_t));
	
	for (int i = 0; i < pcb->tamanio_tabla; i++)
    {   
        segmento = list_get(pcb->tablaDeSegmentos, i);
        buffer_pack(buffer, &(segmento->pid),         sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->id_segmento), sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->base),        sizeof(uint32_t));
        buffer_pack(buffer, &(segmento->tamanio),     sizeof(uint32_t));
        log_error(logger, "pid: %d, id: %d, base: %d, tam: %d",	segmento->pid, segmento->id_segmento, segmento->base, segmento->tamanio);
    }

	stream_send_buffer(conexion_con_cpu, CE, ce_a_enviar);
	log_error(logger, "Tamaño del CE enviado a CPU %d", ce_a_enviar->size);

	buffer_destroy(ce_a_enviar);
}


/*
typedef struct
{
	t_contextoEjecucion* contexto;
	//float estimacionProxRafaga;
	struct timespec llegadaReady;
	double estimacion_anterior;
	double real_anterior;
	t_list *tablaDeSegmentos;
	uint32_t tamanio_tabla;
	t_list *taap; //Tabla de Archivos Abiertos del Proceso
	pthread_mutex_t mutex_TAAP;
} t_pcb;

typedef struct
{
	uint32_t socket;
    uint32_t pid;
	uint32_t program_counter;
	t_instrucciones *instrucciones;
	t_registrosCPU *registrosCPU;
    t_list *tablaDeSegmentos;
	uint32_t tamanio_tabla;
} t_contextoEjecucion;
*/
