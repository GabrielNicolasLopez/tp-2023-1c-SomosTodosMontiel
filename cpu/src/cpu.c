#include "cpu.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{  
        char* modulo = "cpu"; 
        char* path = "./cfg/";
        char* extension= ".log";
        char buffer1[40] = "./cfg/";
        char buffer2[40];
        char* pathcompleto = strcat(buffer1, modulo);
        pathcompleto = strcat(buffer1, extension);
        char* moduloConExtension = strcat(buffer2, modulo);
        moduloConExtension = strcat(buffer2, extension);
        printf("%s", buffer1);
        t_log* logger = log_create(buffer1, modulo, true, LOG_LEVEL_INFO);
        log_info(logger, "Soy el modulo %s! guarde el logger en la siguiente ruta:%s. %s", modulo, buffer1, mi_funcion_compartida());
        log_destroy(logger);
    } 




    //Creo logger para info
	t_log* logger = log_create(LOG_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

    t_configuracion_CPU configuracion_CPU = leerConfiguracion(logger);

    int server_fd = iniciar_servidor("127.0.0.1", configuracion_CPU.PUERTO_ESCUCHA, logger);
    log_info(logger, "Memoria lista para recibir a los modulos");
    int cliente_fd = esperar_cliente(server_fd, logger);

    while (1) {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op) {
        case MENSAJE:
            recibir_mensaje(cliente_fd);
            break;
        case -1:
            log_error(logger, "Un cliente se desconecto.");
            break;
        default:
            log_warning(logger,"Operacion desconocida. No quieras destruir tu sistema operativo");
            break;
        }
    }

    log_destroy(logger); 
}


t_configuracion_CPU leerConfiguracion(t_log* logger){

	//Creo el config para leer IP y PUERTO
	t_config* config = config_create(CONFIG_PATH);

	//Creo el archivo config
	t_configuracion_CPU configuracion_CPU;

	
	configuracion_CPU.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");

	return configuracion_CPU;


}