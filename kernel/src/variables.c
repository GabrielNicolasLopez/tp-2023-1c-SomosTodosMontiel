#include "kernel.h"

t_config *config;
t_kernel_config *configuracionKernel;
t_list* lista_de_recursos;
t_list *tgaa; //Tabla General Archivos Abiertos
int PID_PCB = -1;

int conexion_con_memoria;
int conexion_con_cpu;
int conexion_con_fs;

t_list *LISTA_NEW;
t_list *LISTA_READY;
t_list *LISTA_EXEC;
t_list *LISTA_BLOCKED;
t_list *LISTA_EXIT;
t_list *LISTA_TGAA;

pthread_mutex_t PID;
pthread_mutex_t listaNew;
pthread_mutex_t listaReady;
pthread_mutex_t listaExec;
pthread_mutex_t listaBlocked;
pthread_mutex_t listaExit;
pthread_mutex_t listaTGAA;
pthread_mutex_t mutexFS;

sem_t esPosibleCompactar;
sem_t CantPCBNew;
sem_t cantPCBReady;
sem_t multiprogramacion;
sem_t CPUVacia;
sem_t pasar_pcb_a_CPU;

bool se_reenvia_el_contexto = false;

t_segmento *segmento0;

t_Kernel_Consola razon;
t_FS_header respuesta_fs;
t_entradaTAAP *entradaTAAP;
//t_Kernel_Memoria respuesta_memoria;