#include "kernel.h"

void recibir_cym_desde_cpu(t_motivoDevolucion *motivoDevolucion, int conexion_con_cpu)
{

	uint8_t header = stream_recv_header(conexion_con_cpu);

	if(header != CYM)
		log_error(logger, "KERNEL RECIBIO UN HEADER DIFERENTE A CYM");

	t_buffer *cym_recibido = buffer_create();

	t_contextoEjecucion *contextoEjecucion = malloc(sizeof(t_contextoEjecucion));
	contextoEjecucion->instrucciones = malloc(sizeof(t_instrucciones));

	contextoEjecucion->registrosCPU            = malloc(sizeof(t_registrosCPU));
	contextoEjecucion->registrosCPU->registroC = malloc(sizeof(t_registroC));
	contextoEjecucion->registrosCPU->registroE = malloc(sizeof(t_registroE));
	contextoEjecucion->registrosCPU->registroR = malloc(sizeof(t_registroR));

	contextoEjecucion->instrucciones->listaInstrucciones = list_create();

	motivoDevolucion->contextoEjecucion = contextoEjecucion;

	stream_recv_buffer(conexion_con_cpu, cym_recibido);
	log_error(logger, "Tamaño de cym recibido de cpu %d", cym_recibido->size);

	// Motivo = tipo de instruccion
	buffer_unpack(cym_recibido, &motivoDevolucion->tipo, sizeof(t_tipoInstruccion));
	// log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	// Cantidad entero = numero entero
	buffer_unpack(cym_recibido, &motivoDevolucion->cant_int, sizeof(uint32_t));
	// log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	// Cantidad entero = numero entero
	buffer_unpack(cym_recibido, &motivoDevolucion->cant_intB, sizeof(uint32_t));
	// log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	// Longitud de la cadena
	buffer_unpack(cym_recibido, &motivoDevolucion->longitud_cadena, sizeof(uint32_t));
	// log_debug(logger, "long: %d", motivoDevolucion->longitud_cadena);
	// log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	// Cadena
	motivoDevolucion->cadena = malloc(motivoDevolucion->longitud_cadena);
	buffer_unpack(cym_recibido, motivoDevolucion->cadena, motivoDevolucion->longitud_cadena);
	// log_error(logger, "TAMAÑO DEL BUFFER %d", cym_recibido->size);

	// Socket
	buffer_unpack(cym_recibido, &contextoEjecucion->socket, sizeof(uint32_t));
	// log_debug(logger, "socket: %d", contextoEjecucion->socket);

	// PID
	buffer_unpack(cym_recibido, &contextoEjecucion->pid, sizeof(uint32_t));
	// log_debug(logger, "pid: %d", contextoEjecucion->pid);

	// PC
	buffer_unpack(cym_recibido, &contextoEjecucion->program_counter, sizeof(uint32_t));
	// log_debug(logger, "program_counter: %d", contextoEjecucion->program_counter);

	t_tipoInstruccion instruccion;
	t_instruccion *instruccionRecibida;
	int cantidad_de_instrucciones = 0;
	// Empiezo a desempaquetar las instrucciones
	buffer_unpack(cym_recibido, &instruccion, sizeof(t_tipoInstruccion));

	while (instruccion != EXIT)
	{
		instruccionRecibida = malloc(sizeof(t_instruccion));

		instruccionRecibida->tipo = instruccion;
		// log_debug(logger, "tipo de instruccion: %d", instruccion);

		if (instruccion == DELETE_SEGMENT)
			// Numero de segmento
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		if (instruccion == CREATE_SEGMENT)
		{
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			// Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if (instruccion == SIGNAL)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if (instruccion == WAIT)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			// log_error(logger, "long: %d", instruccionRecibida->longitud_cadena);
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if (instruccion == F_TRUNCATE)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if (instruccion == F_WRITE)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			// Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if (instruccion == F_READ)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			// Parametro B
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntB, sizeof(uint32_t));
		}
		if (instruccion == F_SEEK)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if (instruccion == F_CLOSE)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if (instruccion == F_OPEN)
		{
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if (instruccion == IO)
		{
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if (instruccion == SET)
		{
			// Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			// Longitud cadena
			buffer_unpack(cym_recibido, &instruccionRecibida->longitud_cadena, sizeof(uint32_t));
			instruccionRecibida->cadena = malloc(instruccionRecibida->longitud_cadena);
			// Cadena
			buffer_unpack(cym_recibido, instruccionRecibida->cadena, instruccionRecibida->longitud_cadena);
		}
		if (instruccion == MOV_IN)
		{
			// Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
		}
		if (instruccion == MOV_OUT)
		{
			// Parametro A
			buffer_unpack(cym_recibido, &instruccionRecibida->paramIntA, sizeof(uint32_t));
			// Registro
			buffer_unpack(cym_recibido, &instruccionRecibida->registro, sizeof(t_registro));
		}

		// Agregamos la instruccion recibida a la lista de instrucciones
		list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
		cantidad_de_instrucciones++;

		// Obtengo una nueva instruccion
		buffer_unpack(cym_recibido, &instruccion, sizeof(t_tipoInstruccion));
	}
	instruccionRecibida = malloc(sizeof(t_instruccion));
	// Instruccion EXIT
	instruccionRecibida->tipo = instruccion;
	// log_debug(logger, "tipo de instruccion: %d", instruccion);
	list_add(contextoEjecucion->instrucciones->listaInstrucciones, instruccionRecibida);
	cantidad_de_instrucciones++;
	// Asignamos la cantidad de instrucciones
	contextoEjecucion->instrucciones->cantidadInstrucciones = cantidad_de_instrucciones;

	//Registros
	buffer_unpack(cym_recibido, contextoEjecucion->registrosCPU->registroC, sizeof(t_registroC));
	buffer_unpack(cym_recibido, contextoEjecucion->registrosCPU->registroE, sizeof(t_registroE));
	buffer_unpack(cym_recibido, contextoEjecucion->registrosCPU->registroR, sizeof(t_registroR));

	buffer_unpack(cym_recibido, &contextoEjecucion->tamanio_tabla, sizeof(uint32_t));

	t_segmento *segmento;
    for (int i = 0; i < contextoEjecucion->tamanio_tabla; i++)
    {   
		segmento = malloc(sizeof(t_segmento)); 
		buffer_unpack(cym_recibido, &(segmento->pid),         sizeof(uint32_t));
        buffer_unpack(cym_recibido, &(segmento->id_segmento), sizeof(uint32_t));
        buffer_unpack(cym_recibido, &(segmento->base),        sizeof(uint32_t));
        buffer_unpack(cym_recibido, &(segmento->tamanio),     sizeof(uint32_t));
        list_add(contextoEjecucion->tablaDeSegmentos, segmento);
		log_debug(logger, "pid: %d, id: %d, base: %d, tam: %d",
		segmento->pid,
		segmento->id_segmento,
		segmento->base,
		segmento->tamanio);
    }

	buffer_destroy(cym_recibido);
}