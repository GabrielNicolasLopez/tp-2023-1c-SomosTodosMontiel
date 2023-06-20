#include "filesystem.h"

// *** HILO MEMORIA ***
void* crear_hilo_memoria()
{
    int* int_return = malloc(sizeof(int));
    *int_return = 0;
    return int_return;
}