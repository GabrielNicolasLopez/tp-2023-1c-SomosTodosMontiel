#include "comunicacion.h"

void hilo_cpu(){
    
    while(1){
    log_info(logger, "Esperando paquete de CPU...\n");
    uint8_t header = stream_recv_header(conexion_con_cpu);
		if(header==MOV_IN)
        {
            log_info(logger, "Mi cod de op es: %d", header);
            uint32_t *dir_fisica = NULL;
            uint32_t *dato = NULL;
            pedidoLectura_CPU(dir_fisica);
            sleep((configuracionMemoria->retardo_memoria/1000));
            leer_CPU(dato, *dir_fisica);
            enviarDato_CPU(*dato);
        }
        else if (header==MOV_OUT)
        {
            log_info(logger, "Mi cod de op es: %d", header);
            uint32_t *cantBytes = NULL;
            uint32_t *dato = NULL;
            uint32_t *dir_fisica = NULL;
            pedidoEscritura_CPU(cantBytes,dir_fisica,dato);
            sleep((configuracionMemoria->retardo_memoria/1000));
            escribir_CPU(dato,*dir_fisica,*cantBytes);
            ok_CPU();
        }
        else
            log_error(logger, "CPU envio un header incorrecto: %d", header);
	}	
}


void leer_CPU(uint32_t* dato, uint32_t dirF){
	

    memcpy(dato, espacioUsuario+dirF, sizeof(uint32_t));

    log_debug(logger, "PID: - Acción: LEER - Dirección física: %d - Tamaño: %ld - Origen: CPU",dirF,sizeof(dato));

}

void escribir_CPU(uint32_t* dato , uint32_t dirF,uint32_t cantBytes){

    memcpy(espacioUsuario+dirF,dato,cantBytes);

    log_debug(logger, "PID: - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: CPU",dirF,cantBytes);
}

void pedidoLectura_CPU(uint32_t *dir_fisica)
{
    
    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    //buffer_unpack(buffer, cantBytes, sizeof(uint32_t));

    buffer_destroy(buffer);
}

void pedidoEscritura_CPU(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));    
    // DATO
    buffer_unpack(buffer, dato, *cantBytes);
    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));


    buffer_destroy(buffer);
    
}

void enviarDato_CPU(uint8_t dato){

    t_buffer* buffer =buffer_create();
   // buffer_pack(buffer,OK,sizeof(t_CPU_memoria));
    //buffer_pack(buffer,(sizeof(dato)),sizeof(uint32_t)); //Creo que no iria
    buffer_pack(buffer,&dato,sizeof(uint8_t));
    t_CPU_memoria header = OK;
    stream_send_buffer(conexion_con_cpu, header, buffer);
    buffer_destroy(buffer);
}

void ok_CPU(){
    t_buffer* buffer = buffer_create();
    buffer_pack(buffer, OK, sizeof(t_instruccion));
    stream_send_buffer(conexion_con_cpu, OK, buffer);
    buffer_destroy(buffer);
}