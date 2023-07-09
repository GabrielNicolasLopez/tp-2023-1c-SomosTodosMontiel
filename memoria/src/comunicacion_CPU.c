#include <comunicacion.h>
#include <CPU_memoria.h>


void hilo_cpu(){
        while(1){
    log_info(logger, "Estoy esperando paquete, soy memoria\n");
	
    uint8_t header = stream_recv_header(conexion_con_cpu);

		if(header==MOV_IN)
        {
            log_info(logger, "Mi cod de op es: %d", header);

            uint32_t *dir_fisica;
            uint32_t *dato;
            pedidoLectura(dir_fisica);

            sleep((configuracionMemoria->retardo_memoria/1000));

            leer(&dato,dir_fisica);

            enviarDato(dato);

            return 0;
        }

        else if (header==MOV_OUT)
        {
            log_info(logger, "Mi cod de op es: %d", header);

            uint32_t *cantBytes;
            uint32_t *dir_fisica;
            uint32_t *dato;
            pedidoEscritura(cantBytes,dir_fisica,dato);

            sleep((configuracionMemoria->retardo_memoria/1000));

            escribir(dato,dir_fisica,cantBytes);

            ok();

        return 0;
        }

        else

        return 0;

        
	}	
}


void leer(uint32_t dato , uint32_t dirF){
	

    memcpy(dato,espacioUsuario+dirF,sizeof(dato));

    log_debug(logger, "PID: - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: CPU",dirF,sizeof(dato));

}

void escribir(uint32_t dato , uint32_t dirF,uint32_t cantBytes){

    memcpy(espacioUsuario+dirF,dato,cantBytes);

    log_debug(logger, "PID: - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: CPU",dirF,cantBytes);
}

void pedidoLectura(/*uint32_t *cantBytes,*/ uint32_t *dir_fisica)
{
    
    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    //buffer_unpack(buffer, cantBytes, sizeof(uint32_t));

    buffer_destroy(buffer);
}

void pedidoEscritura(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));    
    // DATO
    buffer_unpack(buffer,dato,cantBytes);
    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));


    buffer_destroy(buffer);
    
}

void enviarDato(uint8_t dato){

    t_buffer* buffer =buffer_create();
   // buffer_pack(buffer,OK,sizeof(t_CPU_memoria));
    buffer_pack(buffer,(sizeof(dato)),sizeof(uint32_t));
    buffer_pack(buffer,&dato,sizeof(uint8_t));
    stream_send_buffer(conexion_con_cpu, OK, buffer);
    buffer_destroy(buffer);
}

void ok(){

    t_buffer* buffer =buffer_create();

    buffer_pack(buffer,OK,sizeof(t_instruccion));
    stream_send_buffer(conexion_con_FileSystem, OK, buffer);
    buffer_destroy(buffer);
}