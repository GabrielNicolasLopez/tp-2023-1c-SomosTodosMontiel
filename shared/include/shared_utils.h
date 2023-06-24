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
#include <semaphore.h>
#include <commons/string.h>
#include <pthread.h>
#include <time.h>

extern char *razonFinConsola[];
extern char *nombresCodigoOperaciones[];
extern char *nombresInstrucciones[];
extern char *nombresRegistros[];

typedef struct 
{
    uint32_t id_segmento;
    uint32_t base;
    uint32_t tamanio;
    //int libre;
}t_segmento;

typedef enum
{
    CE,
    CYM
} t_Kernel_CPU;

typedef enum
{
    OK,
    SIN_MEMORIA,
    NECESITO_COMPACTAR,
    EMPEZA_A_COMPACTAR,
    FIN_COMPACTACION,
    INSTRUCCION
} t_Kernel_Memoria;

/*typedef enum
{
} t_Kernel_FS;*/

typedef enum
{
	MENSAJE,
	PAQUETE,
	NEW
}op_code;

typedef enum
{
    // 4 bytes
    AX,
    BX,
    CX,
    DX,
    // 8 bytes
    EAX,
    EBX,
    ECX,
    EDX,
    // 16 bytes
    RAX,
    RBX,
    RCX,
    RDX
} t_registro;

typedef enum
{
    // 3 parámetros
    F_READ,
    F_WRITE,
    // 2 parámetros
    SET,
    MOV_IN,
    MOV_OUT,
    F_TRUNCATE,
    F_SEEK,
    CREATE_SEGMENT,
    // 1 parámetros
    IO,
    WAIT,
    SIGNAL,
    F_OPEN,
    F_CLOSE,
    DELETE_SEGMENT,
    // 0 parámetros
    YIELD,
    EXIT,
} t_tipoInstruccion;

typedef enum
{
    FIN,
    OUT_OF_MEMORY,
    E_SIGNAL,
    E_WAIT
} t_razonFinConsola;



typedef struct
{
    t_tipoInstruccion tipo;
    t_registro registro;       
    uint32_t paramIntA, paramIntB; // En caso de que se deban guardar dos int
    uint32_t longitud_cadena;
    char *cadena;     
}t_instruccion;

typedef struct
{
	t_list *listaInstrucciones;
	uint32_t cantidadInstrucciones;
}t_instrucciones;

typedef struct
{
	char ax[4];
	char bx[4];
	char cx[4];
	char dx[4];
}__attribute__((packed)) t_registroC;

typedef struct
{
	char eax[8];
	char ebx[8];
	char ecx[8];
	char edx[8];
}__attribute__((packed)) t_registroE;

typedef struct
{
	char rax[16];
	char rbx[16];
	char rcx[16];
	char rdx[16];
}__attribute__((packed)) t_registroR;

typedef struct
{
	t_registroC *registroC;
	t_registroE *registroE;
	t_registroR *registroR;
}t_registrosCPU;

typedef struct
{
    uint32_t prueba;
}entrada_tablaDeSegmentos;

typedef struct
{
	uint32_t socket;
    uint32_t pid;
	uint32_t program_counter;
	t_instrucciones *instrucciones;
	t_registrosCPU *registrosCPU;
} t_contextoEjecucion;

typedef struct
{
    t_tipoInstruccion tipo;
    uint32_t cant_int;
    uint32_t cant_intB;
    uint32_t longitud_cadena;
    char *cadena;
    t_contextoEjecucion *contextoEjecucion;
} t_motivoDevolucion;

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

extern t_log* logger;

void* recibir_buffer(uint32_t*, int);

//int iniciar_servidor(void);
int iniciar_servidor(char *IP, char *PUERTO, t_log* logger);
int esperar_cliente(int socket_cliente, t_log* logger);
//t_list* recibir_paquete(int socket_cliente);
//void recibir_mensaje(int socket_cliente);
int recibir_operacion(int socket_cliente);
int crear_conexion(char *ip, char *puerto, t_log* logger);
void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(void);
t_paquete *crear_super_paquete(void);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
//t_buffer *cargar_buffer_a_t_pcb(t_pcb t_pcb);
void cargar_buffer_a_paquete(t_buffer *buffer, int conexion);
//t_pcb *deserializar_pcb(t_buffer *buffer);
//void deserializar_paquete(int conexion);
//void serializarPCB(int socket, t_pcb *pcb, t_tipoMensaje tipoMensaje);
//void crearPaquete(t_buffer *buffer, t_tipoMensaje op, int unSocket);
//t_paqueteActual *recibirPaquete(int socket);
//t_pcb *deserializoPCB(t_buffer *buffer);
//int calcularSizeInfo(t_informacion* );
//t_instrucciones recibir_instruciones_consola(int cliente_fd);
char* mi_funcion_compartida();
int enviarMensaje(int socket, char *msj);
void *serializarMensaje(char *msj, size_t *size_stream);
int enviarStream(int socket, void *stream, size_t stream_size);
t_motivoDevolucion* deserializar_contexto_y_motivo(t_buffer* buffer);


// ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼ FUNCIONES CPU - KERNEL - (consola) ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼

// LIBERAR UNA INSTRUCCIONS
void instruccion_destroy(t_instruccion* instruccion);
// LIBERAR CONTEXTO DE EJECUCION
void contexto_de_ejecucion_destroy(t_contextoEjecucion* c_e);
// CALCULAR TAMAÑOS PARA BUFFERS
int calcularSizeListaInstrucciones(t_instrucciones *instrucciones);

// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲ FUNCIONES CPU - KERNEL - (consola) ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲


#endif