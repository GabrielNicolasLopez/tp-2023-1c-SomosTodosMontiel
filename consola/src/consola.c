#include "consola.h"

#define CONSOLA_CONFIG_PATH "./cfg/consola.cfg"
#define CONSOLA_LOG_PATH "./cfg/consola.log"
#define CONSOLA_MODULE_NAME "consola"

int main(int argc, char* argv[]){


    t_log *logger = log_create(CONSOLA_LOG_PATH, CONSOLA_MODULE_NAME, true, LOG_LEVEL_INFO);

    t_config* config = config_create(CONSOLA_CONFIG_PATH);
    char* ip = config_get_string_value(config, "IP");
    char* puerto = config_get_string_value(config, "PUERTO");


    log_info(logger, "La IP es %s", ip);
    log_info(logger, "El puerto es %s", puerto);
    log_info(logger, "EL argc es %d", argc);
    for(int i =0;i<10; i++){
    log_info(logger, "EL argv[%d] es %s",i, argv[i]);
    }
    log_destroy(logger);

    int i;
    printf("argc = %d\n", argc);

    printf("Vamos a ver que hay en argv[]\n");

    for(i=0; i<argc+1; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}