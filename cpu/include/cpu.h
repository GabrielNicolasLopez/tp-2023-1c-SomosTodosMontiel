#ifndef CPU_H_
#define CPU_H_

#include "shared_utils.h"
#include "tests.h"
#include "buffer.h"
#include "stream.h"

//#define LOG_PATH "./cfg/cpu.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/cpuPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "CPU"

typedef struct
{
	char* puerto_escucha;
	char* ip_memoria;
	char* puerto_memoria;
	int retardo_instruccion;
}t_cpu_config;

extern t_cpu_config *configuracion_cpu;

// OBTENEMOS LA CONFIGURACION DEL PROCESO
t_cpu_config* leer_configuracion(t_config* config);

// CREA LOS HILOS QUE VA A USAR LA CPU
void crear_hilos_cpu();

// *** HILO MEMORIA ***
void crear_hilo_memoria();

// *** HILO KERNEL ***
void crear_hilo_kernel();

// CICLO DE INSTRUCCION DEL CPU
t_contextoEjecucion* ciclo_instruccion(t_contextoEjecucion* contexto_ejecucion, int cliente_fd_kernel, bool* enviamos_CE_al_kernel);

// -- SERIALIZANDO Y DESERIALIZANDO --

// SERIALIZA MOTIVO DE DEVOLUCION Y CONTEXTO DE EJECUCION EN PAQUETE A ENVIAR
void armar_buffer_motivo_y_contexto(t_buffer* buffer, t_motivoDevolucion motivo_devolucion, t_contextoEjecucion* contexto_ejecucion);
// EMPAQUETAMOS Y ENVIAMOS EL CONTEXTO DE EJECUCION INDICANDO EL ESCENARIO (MOTIVO DE DEVOLUCION) AL KERNEL
void enviar_contexto_y_motivo(t_motivoDevolucion motivo_devolucion, t_contextoEjecucion* contexto_ejecucion, int server_fd);
// OBTENEMOS EL CONTEXTO DE EJECICIÓN
t_contextoEjecucion* recibir_contexto(int socket_cliente);
// DESERIALIZAMOS CONTEXTO RECIBIDO
t_contextoEjecucion* deserializar_contexto(t_buffer* buffer);

// -- SERIALIZANDO Y DESERIALIZANDO --


t_contextoEjecucion* recibir_ce_de_kernel(int cliente_fd_kernel);
void enviar_cym_a_kernel(t_motivoDevolucion motivo, t_contextoEjecucion *contextoEjecucion, int cliente_fd_kernel);

void hilo_kernel();
void hilo_memoria();

#endif