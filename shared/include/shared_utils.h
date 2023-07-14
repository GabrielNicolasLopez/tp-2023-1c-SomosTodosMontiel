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

#define IP_SERVER "0.0.0.0"

extern char *razonFinConsola[];
extern char *nombresCodigoOperaciones[];
extern char *nombresInstrucciones[];
extern char *nombresRegistros[];

typedef enum {
    HEADER_fin,
    HEADER_error_out_of_memory,
    HEADER_error_recurso,
    HEADER_lista_instrucciones,
    HEADER_contexto_ejecucion,
    HEADER_instruccion,
    HEADER_segmentation_fault
} t_header;

typedef enum {
    HANDSHAKE_consola,
    HANDSHAKE_cpu,
    HANDSHAKE_filesystem,
    HANDSHAKE_kernel,
    HANDSHAKE_ok_continue,
    HANDSHAKE_error
} t_handshake;

typedef struct 
{
    uint32_t id_segmento;
    uint32_t base;
    uint32_t tamanio;
    uint32_t pid;
}t_segmento;

typedef enum
{
    INSTRUCCIONES,
    FIN,
    OUT_OF_MEMORY,
    E_SIGNAL,
    E_WAIT
} t_Kernel_Consola;

typedef enum
{
    CE,
    CYM
} t_Kernel_CPU;

typedef enum
{
    OK
} t_CPU_memoria;

typedef enum
{
    //OK,
    SIN_MEMORIA,
    NECESITO_COMPACTAR,
    EMPEZA_A_COMPACTAR,
    FIN_COMPACTACION,
    INSTRUCCION, 
    BASE,
    LISTA,
    TAMANIO,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    PROCESO_BORRADO
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
    F_CREATE,
    F_CLOSE,
    DELETE_SEGMENT,
    // 0 parámetros
    YIELD,
    EXIT,
    SEG_FAULT
} t_tipoInstruccion;

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
    uint32_t base;
    uint32_t tamanio;
}t_hueco;

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
    t_list *tablaDeSegmentos;
	uint32_t tamanio_tabla;
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

int iniciar_servidor(char *IP, char *PUERTO);
int esperar_cliente(int socket_cliente);

int crear_conexion(char *ip, char *puerto);


void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);



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