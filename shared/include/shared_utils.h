#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h>

//#define IP "127.0.0.1"
//#define PUERTO "4444"

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	uint32_t size; // Tamaño del payload
	void *stream;  // Payload
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer *buffer;
} t_paquete;


typedef struct
{
	int hola;
} t_pcb;

extern t_log* logger;

void* recibir_buffer(int*, int);

//int iniciar_servidor(void);
int iniciar_servidor(char *IP, char *PUERTO, t_log* logger);
int esperar_cliente(int, t_log*);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
int crear_conexion(char *ip, char *puerto, t_log* logger);
void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(void);
t_paquete *crear_super_paquete(void);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
t_buffer *cargar_buffer_a_t_pcb(t_pcb t_pcb);
void cargar_buffer_a_paquete(t_buffer *buffer, int conexion);
//t_pcb *deserializar_pcb(t_buffer *buffer);
//void deserializar_paquete(int conexion);
//void serializarPCB(int socket, t_pcb *pcb, t_tipoMensaje tipoMensaje);
//void crearPaquete(t_buffer *buffer, t_tipoMensaje op, int unSocket);
//t_paqueteActual *recibirPaquete(int socket);
//t_pcb *deserializoPCB(t_buffer *buffer);
//int calcularSizeInfo(t_informacion* );

char* mi_funcion_compartida();

#endif