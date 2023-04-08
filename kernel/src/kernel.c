#include "kernel.h"

#define CONFIG_PATH "./cfg/kernel.cfg"
#define LOG_PATH "./cfg/kernel.log"
#define MODULE_NAME "kernel"


int main(void){

    int conexion;
	char* ip;
	char* puerto;
    t_log *logger;
    t_config* config;

    logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

}