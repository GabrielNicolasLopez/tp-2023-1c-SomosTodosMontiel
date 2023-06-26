#include "superbloque.h"

// SUPERBLOQUE
t_config* t_config_SupBloque;
t_superbloque_config* config_SupBloque;


int levantar_volumen()
{
    levantar_superbloque();

    if(!existe_archivo(configFS->PATH_BITMAP)) {
        log_info(logger, "Bitmap inexistente, generando archivos...");
        crear_bitmap();
        crear_archivo_de_bloques();
        if (crear_carpeta_de_FCBs() == -1) {
            return -1;
        }

    }

    levantar_bitmap();
    levantar_bloques();
    
    return 0;
}

void levantar_superbloque()
{
    log_info(logger, "Levantando ARCHIVO DE SUPERBLOQUE"); 
    t_config_SupBloque = config_create(configFS->PATH_SUPERBLOQUE);
    config_SupBloque = leerConfiguracion_superbloque();
}

// CONFIGURACION SUPERBLOQUE
t_superbloque_config* leerConfiguracion_superbloque() 
{
    t_superbloque_config* configuracion = malloc(sizeof(t_superbloque_config));

	configuracion->BLOCK_SIZE = config_get_int_value(t_config_SupBloque, "BLOCK_SIZE");
    configuracion->BLOCK_COUNT = config_get_int_value(t_config_SupBloque, "BLOCK_COUNT");

	return configuracion;
}

bool existe_archivo(const char* ruta_archivo)
{
    FILE *file = fopen(ruta_archivo, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}



