#include <memoria.h>
#include <funciones.h>

//Crear segmentos 0


   crear_segmento(0,0,150);





//Crear estrcuturas globales para un proceso 

/*t_segmentos_por_proceso * crear_lista(aca tendria que recibir el pid o el pcb){

    t_segemntos_por_proceso* lista;



}
*/
//Creación de Segmento

t_segmento* crear_segmento(int id, int base, int tam){

    t_segmento* segmento;
    malloc(sizeof(t_segmento)); 

    segmento ->id_segmento = id;
    segmento ->base = base;
    segmento ->tamanio = tam;
     
    return segmento;
}

//Eliminación de Segmento

void * eliminnar_segmento(t_segmento* segmento){

    free(segmento);
}

//Compactación de Segmentos



