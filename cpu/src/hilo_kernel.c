#include <cpu.h>

// CICLO DE INSTRUCCION DEL CPU
t_contextoEjecucion* ciclo_instruccion(t_contextoEjecucion* contexto_ejecucion, int cliente_fd_kernel, bool* enviamos_CE_al_kernel)
{
	// FETCH
	t_instruccion* instruccion = list_get(contexto_ejecucion -> instrucciones -> listaInstrucciones, contexto_ejecucion -> program_counter); 	

	// DECODE and EXECUTE
	t_motivoDevolucion motivo = {
		.longitud_cadena = 0,
		.cadena = "",
		.cant_int = 0,
	};
	switch(instruccion -> tipo){
		// 3 PARAMETROS
		case F_READ:	// F_READ (Nombre Archivo, Dirección Lógica, Cantidad de Bytes):
						// Esta instrucción solicita al Kernel que se lea del archivo indicado, la cantidad de bytes pasada
						// por parámetro y se escriba en la dirección física de Memoria la información leída.
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %s - %u", 
				contexto_ejecucion->pid, 
				nombresInstrucciones[instruccion->tipo],
				//falta la dirección lógica
				instruccion->cadena,
				instruccion->paramIntA);


			break;
		case F_WRITE:	// F_WRITE (Nombre Archivo, Dirección Lógica, Cantidad de bytes): 
						// Esta instrucción solicita al
						// Kernel que se escriba en el archivo indicado, la cantidad de bytes pasada por parámetro cuya
						// información es obtenida a partir de la dirección física de Memoria.
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %s - %u", 
				contexto_ejecucion->pid, 
				nombresInstrucciones[instruccion->tipo],
				//falta la dirección lógica
				instruccion->cadena, 
				instruccion->paramIntA);


			break;
		
		// 2 PARAMETROS
		case SET: 	// SET (Registro, Valor): 
					// Asigna al registro el valor pasado como parámetro
			log_info(logger, "entre");
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %s - %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo],
				nombresRegistros[instruccion->registro],
				instruccion->cadena);

			contexto_ejecucion -> program_counter++;
			sleep((configuracion_cpu -> retardo_instruccion)/1000);
			switch(instruccion -> registro){
				// registros de tamaño 4
				case AX: memcpy(contexto_ejecucion -> registrosCPU -> registroC -> ax, instruccion -> cadena, 4);
				log_debug(logger, "El registro AX = %.4s", contexto_ejecucion -> registrosCPU -> registroC -> ax); 
				break;
				case BX: memcpy(contexto_ejecucion -> registrosCPU -> registroC -> bx, instruccion -> cadena, 4);
				log_debug(logger, "El registro BX = %.4s", contexto_ejecucion -> registrosCPU -> registroC -> bx); 
				break;
				case CX: memcpy(contexto_ejecucion -> registrosCPU -> registroC -> cx, instruccion -> cadena, 4);
				log_debug(logger, "El registro CX = %.4s", contexto_ejecucion -> registrosCPU -> registroC -> cx); 
				break;
				case DX: memcpy(contexto_ejecucion -> registrosCPU -> registroC -> dx, instruccion -> cadena, 4);
				log_debug(logger, "El registro DX = %.4s", contexto_ejecucion -> registrosCPU -> registroC -> dx); 
				break;
				// registros de tamaño 8
				case EAX: memcpy(contexto_ejecucion -> registrosCPU -> registroE -> eax, instruccion -> cadena, 8);
				log_debug(logger, "El registro EAX = %.8s", contexto_ejecucion -> registrosCPU -> registroE -> eax); 
				break;
				case EBX: memcpy(contexto_ejecucion -> registrosCPU -> registroE -> ebx, instruccion -> cadena, 8);
				log_debug(logger, "El registro EBX = %.8s", contexto_ejecucion -> registrosCPU -> registroE -> ebx); 
				break;
				case ECX: memcpy(contexto_ejecucion -> registrosCPU -> registroE -> ecx, instruccion -> cadena, 8);
				log_debug(logger, "El registro ECX = %.8s", contexto_ejecucion -> registrosCPU -> registroE -> ecx); 
				break;
				case EDX: memcpy(contexto_ejecucion -> registrosCPU -> registroE -> edx, instruccion -> cadena, 8);
				log_debug(logger, "El registro EDX = %.8s", contexto_ejecucion -> registrosCPU -> registroE -> edx); 
				break;
				// registros de tamaño 16
				case RAX: memcpy(contexto_ejecucion -> registrosCPU -> registroR -> rax, instruccion -> cadena, 16);
				log_debug(logger, "El registro AX = %.16s", contexto_ejecucion -> registrosCPU -> registroR -> rax); 
				break;
				case RBX: memcpy(contexto_ejecucion -> registrosCPU -> registroR -> rbx, instruccion -> cadena, 16);
				log_debug(logger, "El registro AX = %.16s", contexto_ejecucion -> registrosCPU -> registroR -> rbx); 
				break;
				case RCX: memcpy(contexto_ejecucion -> registrosCPU -> registroR -> rcx, instruccion -> cadena, 16);
				log_debug(logger, "El registro AX = %.16s", contexto_ejecucion -> registrosCPU -> registroR -> rcx); 
				break;
				case RDX: memcpy(contexto_ejecucion -> registrosCPU -> registroR -> rdx, instruccion -> cadena, 16);
				log_debug(logger, "El registro AX = %.16s", contexto_ejecucion -> registrosCPU -> registroR -> rdx); 
				break;
			}
			break;
		case MOV_IN: 	// MOV_IN (Registro, Dirección Lógica): 
						// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro.
			
			break;
		case MOV_OUT: 	// MOV_OUT (Dirección Lógica, Registro): 
						// Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica.
				//MOV_OUT 120 AX
				log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %d - %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo],
				instruccion->paramIntA,
				nombresRegistros[instruccion->registro]);
				contexto_ejecucion -> program_counter++;
			break;
		case F_TRUNCATE: // F_TRUNCATE (Nombre Archivo, Tamaño): 
						 // Esta instrucción solicita al Kernel que se modifique el tamaño del archivo al indicado por parámetro.
			
			break;
		case F_SEEK:	// F_SEEK (Nombre Archivo, Posición): 
						// Esta instrucción solicita al kernel actualizar el puntero del archivo a la posición pasada por parámetro.
			
			break;
		case CREATE_SEGMENT: // CREATE_SEGMENT (Id del Segmento, Tamaño): 
							 // Esta instrucción solicita al kernel la creación del segmento con el Id y tamaño indicado por parámetro.

			break;

		// 1 PARAMETROS
		case IO:	// I/O (Tiempo): Esta instrucción representa una syscall de I/O bloqueante. Se deberá devolver
					// el Contexto de Ejecución actualizado al Kernel junto a la cantidad de unidades de tiempo
					// que va a bloquearse el proceso.
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %u",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo],
				instruccion->paramIntA);

			contexto_ejecucion -> program_counter++;
			motivo.tipo = IO;
			motivo.cant_int = instruccion->paramIntA;
			enviar_cym_a_kernel(motivo, contexto_ejecucion, cliente_fd_kernel);
			*enviamos_CE_al_kernel = true;
			break;
		case WAIT:		// WAIT (recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso
						// indicado por parámetro.
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo],
				instruccion->cadena);
				
            contexto_ejecucion -> program_counter++;
			motivo.tipo = WAIT;
			motivo.longitud_cadena = string_length(instruccion->cadena)+1;
			strcpy(&motivo.cadena, instruccion->cadena);
			enviar_cym_a_kernel(motivo, contexto_ejecucion, cliente_fd_kernel);
			*enviamos_CE_al_kernel = true;
			break;
		case SIGNAL:	// SIGNAL (cadena): Esta instrucción solicita al Kernel que se libere una instancia del cadena
						// indicado por parámetro.
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s - %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo],
				instruccion->cadena);
			
			contexto_ejecucion -> program_counter++;
			motivo.tipo = SIGNAL;
			motivo.longitud_cadena = string_length(instruccion->cadena)+1;
			strncpy(motivo.cadena, instruccion->cadena, motivo.longitud_cadena);
			enviar_cym_a_kernel(motivo, contexto_ejecucion, cliente_fd_kernel);
			*enviamos_CE_al_kernel = true;
            break;
		case F_OPEN: 	// F_OPEN (Nombre Archivo): Esta instrucción solicita al kernel que abra o cree el archivo
					 	// pasado por parámetro.
			break;
		case F_CLOSE:	// F_CLOSE (Nombre Archivo): Esta instrucción solicita al kernel que cierre el archivo pasado
						// por parámetro.
			break;
		case DELETE_SEGMENT:	// DELETE_SEGMENT (Id del Segmento): Esta instrucción solicita al kernel que se elimine el
								// segmento cuyo Id se pasa por parámetro.
			break;

		// 0 PARAMETROS
		case YIELD:
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo]);

			contexto_ejecucion -> program_counter++;
			motivo.tipo = YIELD;
			enviar_cym_a_kernel(motivo, contexto_ejecucion, cliente_fd_kernel);
			*enviamos_CE_al_kernel = true;
			break;
		case EXIT:
			log_info(logger, "Instruccion Ejecutada: PID: %u - Ejecutando: %s",
				contexto_ejecucion->pid,
				nombresInstrucciones[instruccion->tipo]);
				
			motivo.tipo = EXIT;
			enviar_cym_a_kernel(motivo, contexto_ejecucion, cliente_fd_kernel);
			*enviamos_CE_al_kernel = true;
			break;
		default:
	}
	return contexto_ejecucion;
}