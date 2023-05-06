#ifndef KERNEL_H
#define KERNEL_H
#include "shared_utils.h"

#define CONFIG_PATH "./cfg/kernel.cfg"
//#define LOG_PATH "./cfg/kernel.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/kernelPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Kernel"

t_config *config;

typedef struct
{
    //datos para kernel como cliente
    char* IP_MEMORIA;
    char* IP_FILESYSTEM;
    char* IP_CPU;
    char* PUERTO_MEMORIA;
    char* PUERTO_FILESYSTEM;
	char* PUERTO_CPU;
	char* ALGORITMO_PLANIFICACION;
	char* ESTIMACION_INICIAL;
	int HRRN_ALFA;
	int GRADO_MAX_MULTIPROGRAMACION;
	char** RECURSOS;
	char** INSTANCIAS_RECURSOS;
    //datos para kernel como servidor
    char* PUERTO_ESCUCHA;
} t_kernel_config;

t_kernel_config *configuracionKernel;

typedef struct
{
	char *nombre;
	int instancias_recursos;
	t_list *lista_block;
	pthread_mutex_t *mutex_lista_blocked;
    sem_t contador_bloqueo;
}t_recurso;

typedef struct
{
	//Consiga
    uint32_t pid;
	t_instrucciones *instrucciones;
	uint32_t program_counter;
	t_registrosCPU *registrosCPU;
	t_list *tablaDeSegmentos;
	float estimacionProxRafaga;
	struct timespec llegadaReady;
	t_list *taap; //Tabla de Archivos Abiertos del Proceso
	//Agregados de la consigna
	t_contextoEjecucion contexto;
    uint32_t socket;
	float estimacion_anterior;
	float real_anterior;
} t_pcb;

typedef struct
{
	char* inicioArchivo;
}t_entradaTAAP;

typedef struct
{
	int socket;
	t_instrucciones instrucciones;
} t_datosCrearPCB;

typedef enum
{
	FIFO,
	HRRN
} t_tipo_algoritmo;


t_list *tgaa; //Tabla General Archivos Abiertos


t_kernel_config *leerConfiguracion();
void crear_pcb(void *datos);
void crear_hilo_memoria();
void crear_hilo_filesystem();
void crear_hilo_cpu();
void crear_hilo_consola();
void crear_hilos_kernel();
void cargarRecursos();
int enviarMensaje(int socket, char *msj);
void *serializarMensaje(char *msj, size_t *size_stream);
int enviarStream(int socket, void *stream, size_t stream_size);
t_instrucciones recibir_informacion(int cliente_fd);
t_paquete *crear_paquete_contexto_ejecucion(t_pcb *pcb);
void implementar_fifo();
void implementar_hrrn();
void agregar_pcb();
//t_tipo_algoritmo obtenerAlgoritmo();

void pasar_a_new(t_pcb *pcb);
void pasar_a_ready(t_pcb *pcb);

void iniciar_listas_y_semaforos();
void liberar_listas_y_semaforos();

int PID_PCB = -1;

//Estados
t_list *LISTA_NEW;
t_list *LISTA_READY;
t_list *LISTA_EXEC;
t_list *LISTA_BLOCKED;
t_list *LISTA_EXIT;

//MUTEX's
pthread_mutex_t PID;

//Mutex de listas
pthread_mutex_t listaNew;
pthread_mutex_t listaReady;
pthread_mutex_t listaExec;


//Semaforos
sem_t CantPCBNew;
sem_t cantPCBReady;
sem_t multiprogramacion;
sem_t PCBEjecutando;

#endif
