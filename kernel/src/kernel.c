#include "kernel.h"

int main(int argc, char ** argv){
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{  
        char* modulo = "kernel"; 
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
}