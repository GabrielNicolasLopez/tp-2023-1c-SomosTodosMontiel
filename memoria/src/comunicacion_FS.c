#include <comunicacion.h>
#include <FS_memoria.h>


void hilo_filesystem(){
    while(1){
    log_info(logger, "Estoy esperando paquete, soy memoria\n");
	
    uint8_t header = stream_recv_header(conexion_con_FileSystem);

		if(header==FS_M_READ)
        {
            log_info(logger, "Mi cod de op es: %d", header);

            uint32_t *cantBytes;
            uint32_t *dir_fisica;
            uint32_t *dato;
            pedidoLectura(cantBytes,dir_fisica);

            sleep((configuracionMemoria->retardo_memoria/1000));

            leer(&dato,dir_fisica,cantBytes);

            enviarDato(dato);

            return 0;
        }

        else if (header==FS_M_WRITE)
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
        log_info(logger, "Mi cod de op es: %d", header);
        return 0;

        
	}	
}

void leer(uint32_t dato , uint32_t dirF,uint32_t cantBytes){
	

    memcpy(dato,espacioUsuario+dirF,cantBytes);

    log_debug(logger, "PID: - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: FS",dirF,cantBytes);

}

void escribir(uint32_t dato , uint32_t dirF,uint32_t cantBytes){

    memcpy(espacioUsuario+dirF,dato,cantBytes);

    log_debug(logger, "PID: - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: FS",dirF,cantBytes);
}


void pedidoLectura(uint32_t *cantBytes, uint32_t *dir_fisica)
{
    
    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_FileSystem,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));

    
    buffer_destroy(buffer);
}

void pedidoEscritura(uint32_t *cantBytes, uint32_t *dir_fisica,uint32_t* dato){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_FileSystem,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));
    // DATO
    buffer_pack(buffer,dato,cantBytes);

    buffer_destroy(buffer);
    
}


void enviarDato(uint8_t dato){

    t_buffer* buffer =buffer_create();

    buffer_pack(buffer,FS_M_READ_OK,sizeof(t_FS_MEMORIA));
    buffer_pack(buffer,(sizeof(dato)),sizeof(uint32_t));
    buffer_pack(buffer,&dato,sizeof(uint8_t));
    stream_send_buffer(conexion_con_FileSystem, FS_M_READ_OK, buffer);
    buffer_destroy(buffer);
}

void ok(){

    t_buffer* buffer =buffer_create();

    buffer_pack(buffer,FS_M_WRITE_OK,sizeof(t_FS_MEMORIA));
    stream_send_buffer(conexion_con_FileSystem, FS_M_WRITE_OK, buffer);
    buffer_destroy(buffer);
}