#include "kernel.h"

void enviar_ce_a_cpu(t_contextoEjecucion *contextoEjecucion, int conexion_con_cpu)
{

	t_buffer *ce_a_enviar = buffer_create();

	// Socket
	buffer_pack(ce_a_enviar, &contextoEjecucion->socket, sizeof(uint32_t));
    //log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	// PID
	buffer_pack(ce_a_enviar, &contextoEjecucion->pid, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	// PC
	buffer_pack(ce_a_enviar, &contextoEjecucion->program_counter, sizeof(uint32_t));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

	// Instrucciones
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	for (int i = 0; i < contextoEjecucion->instrucciones->cantidadInstrucciones; i++)
	{
		instruccion = list_get(contextoEjecucion->instrucciones->listaInstrucciones, i);
		// log_debug(logger, "obtuve la instruccion: %d", i+1);
		buffer_pack(ce_a_enviar, &instruccion->tipo, sizeof(t_tipoInstruccion));
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
	buffer_pack(ce_a_enviar, contextoEjecucion->registrosCPU->registroC, sizeof(t_registroC));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	buffer_pack(ce_a_enviar, contextoEjecucion->registrosCPU->registroE, sizeof(t_registroE));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);
	buffer_pack(ce_a_enviar, contextoEjecucion->registrosCPU->registroR, sizeof(t_registroR));
	//log_error(logger, "TAMAÑO DEL BUFFER %d", ce_a_enviar->size);

	uint8_t header = CE;
	stream_send_buffer(conexion_con_cpu, header, ce_a_enviar);
	log_error(logger, "Tamaño del CE enviado a CPU %d", ce_a_enviar->size);

	buffer_destroy(ce_a_enviar);
}