#ifndef HILO_CONSUMIDOR_H_
#define HILO_CONSUMIDOR_H_

    #include "filesystem.h"

    void crear_hilo_consumidor();
    void respuesta_a_kernel(int operacion, t_instruccion_FS* instruccion); 
    void pedido_escritura_mem(uint32_t cantBytes, uint8_t* cadena_bytes, uint32_t dir_fisica);
    void pedido_lectura_mem(uint32_t cantBytes, uint32_t dir_fisica);
    void recibir_cadena_bytes_mem(uint32_t* cantBytes, uint8_t* cadena_bytes);
 

#endif