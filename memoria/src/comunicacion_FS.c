#include "comunicacion.h"
#include "FS_memoria.h"


void hilo_filesystem(){
    while(1){
        log_info(logger, "Estoy esperando paquete, soy memoria\n");
        uint8_t header = stream_recv_header(conexion_con_FileSystem);
            if(header==FS_M_READ)
            {
                log_info(logger, "Mi cod de op es: %d", header);
                uint32_t *cantBytes = NULL;
                uint32_t *dir_fisica = NULL;
                uint32_t *dato = NULL;
                uint32_t *pid = NULL;
                pedidoLectura_FS(cantBytes,dir_fisica,pid);
                sleep((configuracionMemoria->retardo_memoria/1000));
                leer_FS(dato, *dir_fisica, *cantBytes,*pid);
                enviarDato_FS(*dato);
            }

            else if (header==FS_M_WRITE)
            {
                log_info(logger, "Mi cod de op es: %d", header);
                uint32_t *cantBytes = NULL;
                uint32_t *dir_fisica = NULL;
                uint32_t *pid = NULL;
                uint32_t *dato = NULL;
                pedidoEscritura_FS(cantBytes,dir_fisica,dato,pid);
                sleep((configuracionMemoria->retardo_memoria/1000));
                escribir_FS(dato,*dir_fisica,*cantBytes,*pid);
                ok_FS();
            }

            else
                log_info(logger, "FS no envio el header correcto: %d", header);
    }	
}

void leer_FS(uint32_t* dato, uint32_t dirF, uint32_t cantBytes,uint32_t pid){
	
    memcpy(dato,espacioUsuario+dirF,cantBytes);

    log_debug(logger, "PID:%d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: FS",pid,dirF,cantBytes);

}

void escribir_FS(uint32_t* dato, uint32_t dirF,uint32_t cantBytes,uint32_t pid){

    memcpy(espacioUsuario+dirF, dato, cantBytes);

    log_debug(logger, "PID:%d - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: FS",pid,dirF,cantBytes);
}


void pedidoLectura_FS(uint32_t *cantBytes, uint32_t *dir_fisica,uint32_t *pid)
{
    
    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_FileSystem,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));
    //PID
    buffer_unpack(buffer,pid,sizeof(uint32_t));
    
    buffer_destroy(buffer);
}

void pedidoEscritura_FS(uint32_t *cantBytes, uint32_t *dir_fisica,uint32_t* dato,uint32_t *pid){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_FileSystem,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));
    // DATO
    buffer_unpack(buffer,dato,*cantBytes);
    //PID
    buffer_unpack(buffer,pid,sizeof(uint32_t));

    buffer_destroy(buffer);
    
}


void enviarDato_FS(uint8_t dato){

    t_buffer* buffer =buffer_create();

    //buffer_pack(buffer,FS_M_READ_OK,sizeof(t_FS_MEMORIA));
    //buffer_pack(buffer,(sizeof(dato)),sizeof(uint32_t));
    buffer_pack(buffer,&dato,sizeof(uint8_t));
    stream_send_buffer(conexion_con_FileSystem, FS_M_READ_OK, buffer);
    buffer_destroy(buffer);
}

void ok_FS(){

    t_buffer* buffer =buffer_create();

    //buffer_pack(buffer,FS_M_WRITE_OK,sizeof(t_FS_MEMORIA));
    //stream_send_buffer(conexion_con_FileSystem, FS_M_WRITE_OK, buffer);
    stream_send_empty_buffer(conexion_con_FileSystem, FS_M_WRITE_OK);
    buffer_destroy(buffer);
}