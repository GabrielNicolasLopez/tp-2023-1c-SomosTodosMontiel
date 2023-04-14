#include "memoria.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{ 
        char* modulo = "memoria"; 
        char* path = "./cfg/";
        char* extension= ".log";
        char buffer1[40] = "./cfg/";
        char buffer2[40];
        char* pathcompleto = strcat(buffer1, modulo);
        pathcompleto = strcat(buffer1, extension);
        char* moduloConExtension = strcat(buffer2, modulo);
        moduloConExtension = strcat(buffer2, extension);
        printf("%s", buffer1);
        logger = log_create(buffer1, modulo, true, LOG_LEVEL_INFO);
        log_info(logger, "Soy el modulo %s! guarde el logger en la siguiente ruta:%s. %s", modulo, buffer1, mi_funcion_compartida());

        int server_fd = iniciar_servidor("127.0.0.1", "8002");
	    log_info(logger, "Servidor listo para recibir al cliente");
	    int cliente_fd = esperar_cliente(server_fd);

        t_list* lista;
	    while (1) {
            int cod_op = recibir_operacion(cliente_fd);
            switch (cod_op) {
            case MENSAJE:
                recibir_mensaje(cliente_fd);
                break;
            case -1:
                log_error(logger, "el cliente se desconecto. Terminando servidor");
                return 0;
            default:
                log_warning(logger,"Operacion desconocida. No quieras destruir tu sistema operativo");
                break;
            }
        }

        log_destroy(logger);
    } 

}