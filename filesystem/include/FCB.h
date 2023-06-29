#ifndef FCB_H_
#define FCB_H_

    #include "filesystem.h"
    typedef struct
    {
        char* NOMBRE_ARCHIVO;
        uint32_t TAMANIO_ARCHIVO;
        uint32_t PUNTERO_DIRECTO;
        uint32_t PUNTERO_INDIRECTO;

    } t_FCB_config ;

    typedef struct
    {
        char* nombre_archivo;
        t_config* config;
        t_FCB_config* FCB_config;
        
    } t_lista_FCB_config;

    
    int crear_carpeta_de_FCBs();
    t_config* crear_FCB(char* archivo);

    t_FCB_config* leerConfiguracion_FCB(t_config* config_FCB);
    t_FCB_config* levantar_FCB(t_config* t_config_FCB);
    void actualizar_FCB(t_lista_FCB_config* FCB);

    t_config* buscar_FCB(char* archivo);

    t_lista_FCB_config* FCB_list_get(char* archivo);
    

    int asignar_bloques(t_lista_FCB_config* FCB, uint32_t bytes);
    int liberar_bloques(t_lista_FCB_config* FCB, uint32_t bytes);


    uint32_t minimum(uint32_t x, uint32_t y);
    int max(int x, int y);

#endif