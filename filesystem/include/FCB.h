#ifndef SUPERBLOQUE_H_
#define SUPERBLOQUE_H_

    #include "filesystem.h"

    int crear_carpeta_de_FCBs();
    t_config* crear_FCB(char* archivo);

    t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB);
    t_FCB_config* levantar_FCB(t_config* t_config_FCB);

    t_config* buscar_FCB(char* archivo);

    bool es_el_archivo(char* archivo, t_lista_FCB_config* FCB);
    t_lista_FCB_config* FCB_list_get(char* archivo);
    

    int asignar_bloques(t_lista_FCB_config* FCB, uint32_t bytes);

#endif