#ifndef KERNEL_H
#define KERNEL_H

#include "FS_kernel.h"
#include "shared_utils.h"
#include "kernel_cpu.h"
#include "buffer.h"
#include "stream.h"

#define CONFIG_PATH "./cfg/kernel.cfg"
//#define LOG_PATH "./cfg/kernel.log" LOG QUE PERSISTE EN EL REPO REMOTO
#define LOG_PATH "./cfg/kernelPrueba.log" //LOG QUE NO SE SUBE AL REPO REMOTO
#define MODULE_NAME "Kernel"

extern int conexion_con_memoria;
extern int conexion_con_cpu;
extern int conexion_con_fs;

extern t_Kernel_Consola razon;

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
	double ESTIMACION_INICIAL;
	double HRRN_ALFA;
	int GRADO_MAX_MULTIPROGRAMACION;
	char** RECURSOS;
	char** INSTANCIAS_RECURSOS;
    //datos para kernel como servidor
    char* PUERTO_ESCUCHA;
} t_kernel_config;

typedef struct
{
	char *nombre;
	int instancias_recursos;
	t_list *lista_block;
	pthread_mutex_t mutex_lista_blocked;
}t_recurso;

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
	t_pcb *pcb;
	t_motivoDevolucion *motivo;
} t_datosIO;


typedef struct
{
	char* nombreArchivo;
	uint32_t posicionPuntero;
	uint32_t tamanioArchivo;
	t_list *lista_block_archivo;
	pthread_mutex_t mutex_lista_block_archivo;
}t_entradaTGAA;

typedef struct
{
	t_entradaTGAA *entradaTGAA;
}t_entradaTAAP;

typedef struct
{
	int socket;
	t_instrucciones* instrucciones;
} t_datosCrearPCB;

typedef enum
{
	FIFO,
	HRRN
} t_tipo_algoritmo;

//Conexion con modulos
void conectarse_con_cpu();
void conectarse_con_memoria();
void conectarse_con_fs();

//Espera de respuestas de FS
void esperar_respuestas();

//Manejo de archivos
void pasar_a_blocked_de_archivo_de_TGAA(t_pcb *pcb_a_blocked, char* nombre_archivo);
void quitarArchivoEnTGAA(char *nombre_archivo);
void quitarArchivoEnTAAP(t_pcb *pcb, char *nombre_archivo);
void desbloqueo_al_primer_proceso_de_la_cola_del(char *nombre_archivo);
bool hayProcesosEsperandoAl(char *nombre_archivo);
char* recibir_nombre_de_archivo_de_fs();t_pcb* sacar_de_blocked_de_archivo_de_TGAA(char* nombre_archivo);
void actualizar_posicicon_puntero(t_motivoDevolucion *motivoDevolucion);

t_kernel_config *leerConfiguracion();
void crear_pcb(void *datos);
void crear_hilo_memoria();
void crear_hilo_filesystem();
void hilo_general();
void crear_hilo_consola();
void crear_hilos_kernel();
void cargarRecursos();

t_instrucciones *recibir_instruciones_desde_consola(int cliente_fd);
//uint32_t recibir_instruciones_desde_consola(int cliente_fd);
t_paquete *crear_paquete_contexto_ejecucion(t_pcb *pcb);
void implementar_fifo();
void implementar_hrrn();
void agregar_pcb_a_ready();
t_tipo_algoritmo obtenerAlgoritmo();
t_pcb *algoritmo_hrrn(t_list*);
t_pcb *mayorHRRN(t_pcb*,t_pcb*);
t_pcb *algoritmo_fifo(t_list*);
void cargarListaReadyIdPCB(t_list *LISTA_NEW);
void asignarRecurso(char*);
bool existeRecurso(char* recurso);
int recursos_disponibles(char* recurso);

void pasar_a_new(t_pcb *pcb);
void pasar_a_ready(t_pcb *pcb);
void pasar_a_exec(t_pcb *pcb);
void pasar_a_blocked(t_pcb* pcb);
void pasar_a_exit(t_pcb *pcb);

void iniciar_listas_y_semaforos();
void liberar_listas_y_semaforos();
void planiLargoPlazo();
void planiCortoPlazo();
int calcularSizeListaInstrucciones(t_instrucciones *instrucciones);
t_pcb* pcb_ejecutando();
t_pcb* pcb_ejecutando_remove();
void terminar_consola();
t_motivoDevolucion* recibir_contexto_y_motivo(int socket_cliente);
void pasar_a_blocked_de_recurso(t_pcb* pcb, char* nombre_recurso);
void devolverRecurso(char* nombre_recurso);
void update_program_counter(t_pcb *pcb, t_motivoDevolucion *motivoDevolucion);
void devolverContextoEjecucion(t_pcb *pcb, int conexion_con_cpu);

void enviar_ce_a_cpu(t_contextoEjecucion* contextoEjecucion, int conexion_con_cpu);
void devolver_ce_a_cpu(t_contextoEjecucion *contextoEjecucion, int conexion_con_cpu);

void recibir_cym_desde_cpu(t_motivoDevolucion *motivoDevolucion, int conexion_con_cpu);
void devolver_ce_a_cpu(t_contextoEjecucion *contextoEjecucion, int conexion_con_cpu);

void actualizar_pcb(t_contextoEjecucion *contextoEjecucion);

void pedir_a_memoria_que_compacte();

void actualizar_procesos_bloqueados(char *nombre_recurso);

void esperar_respuesta_compactacion();

//---------------------------------MEMORIA---------------------------------
void crear_tabla_de_segmentos(t_pcb *pcb);
void crear_segmento(uint32_t id, uint32_t tamanio);
void agregar_segmento(t_pcb *pcb, t_segmento *segmento_a_agregar);
void eliminar_segmento(uint32_t id);
void recibir_respuesta_create_segment(uint32_t base_segmento, uint32_t id, uint32_t tamanio);



void inicializar_registro_cpu(t_pcb *pcb);

//---------------------------------FS---------------------------------
void enviar_fopen_a_fs(t_motivoDevolucion *motivoDevolucion);
void enviar_fseek_a_fs(t_motivoDevolucion *motivoDevolucion);
void enviar_fread_a_fs(t_motivoDevolucion *motivoDevolucion);
void enviar_fwrite_a_fs(t_motivoDevolucion *motivoDevolucion);
void enviar_ftruncate_a_fs(t_motivoDevolucion *motivoDevolucion);
void enviar_fcreate_a_fs(t_motivoDevolucion *motivoDevolucion);

bool existeEnTGAA(char *nombre_archivo);
t_entradaTGAA* devolverEntradaTGAA(char *nombre_archivo);
void agregarEnTGAA(t_entradaTGAA *entradaTGAA);
void recibir_respuesta_fopen_desde_fs(t_FS_header *respuesta_fs);
void agregarArchivoEnTAAP(char* nombreArchivo);
void agregarArchivoEnTGAA(char* nombreArchivo);
void desbloqueo_del_primer_proceso_de_la_cola_del(char *nombre_archivo);

//void sleep_IO(t_motivoDevolucion *motivoDevolucion);
void sleep_IO(t_datosIO*);

extern t_FS_header *respuesta_fs;

extern t_config *config;
extern t_kernel_config *configuracionKernel;
extern t_list* lista_de_recursos;
extern t_list * LISTA_TGAA; //Tabla General Archivos Abiertos
extern int PID_PCB;

//Estados
extern t_list *LISTA_NEW;
extern t_list *LISTA_READY;
extern t_list *LISTA_EXEC;
extern t_list *LISTA_BLOCKED;
extern t_list *LISTA_EXIT;

//MUTEX's
extern pthread_mutex_t PID;

//Mutex de listas
extern pthread_mutex_t listaNew;
extern pthread_mutex_t listaReady;
extern pthread_mutex_t listaExec;
extern pthread_mutex_t listaBlocked;
extern pthread_mutex_t listaExit;
extern pthread_mutex_t listaTGAA;
extern pthread_mutex_t mutexFS;

//Semaforos
extern sem_t CantPCBNew;
extern sem_t cantPCBReady;
extern sem_t multiprogramacion;
extern sem_t CPUVacia;
extern sem_t pasar_pcb_a_CPU;
extern sem_t esPosibleCompactar;


extern char *nombresCodigoOperaciones[];
extern char *nombresInstrucciones[];

extern bool se_reenvia_el_contexto;

extern t_segmento *segmento0;

#endif