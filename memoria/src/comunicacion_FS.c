#include "comunicacion.h"
#include "FS_memoria.h"


void hilo_filesystem(){
    while(1){
        log_info(logger, "Estoy esperando mensaje de FS...");
        uint8_t header = stream_recv_header(conexion_con_FileSystem);
        log_info(logger, "Mi cod de op es: %d", header);
            if(header==FS_M_READ)
            {
                uint32_t cantBytes;
                uint32_t dir_fisica;
                uint32_t pid;
                char *dato;
                log_info(logger,"Solicitud de Lectura por parte de FS");
                pedidoLectura_FS(&cantBytes, &dir_fisica, &pid);
                //sleep((configuracionMemoria->retardo_memoria/1000));
                dato = malloc(cantBytes);
                leer_FS(dato, dir_fisica, cantBytes, pid);
                enviarDato_FS(cantBytes, dato);
                free(dato);
            }

            else if (header==FS_M_WRITE)
            {
                uint32_t cantBytes;
                uint32_t dir_fisica;
                uint32_t pid;
                log_info(logger,"Solicitud de Escritura por parte de FS");
                char *dato = pedidoEscritura_FS(&cantBytes,&dir_fisica,&pid);
                //sleep((configuracionMemoria->retardo_memoria/1000));
                escribir_FS(dato,dir_fisica,cantBytes,pid);
                free(dato);
                ok_FS();
            }

            else
                log_error(logger, "FS no envio el header correcto: %d", header);
    }	
}

void leer_FS(char* dato, uint32_t dirF, uint32_t cantBytes,uint32_t pid){
	
    memcpy(dato, espacioUsuario+dirF, cantBytes);

    log_info(logger, "PID:%d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: FS",pid ,dirF,cantBytes);

}

void escribir_FS(char* dato, uint32_t dirF,uint32_t cantBytes,uint32_t pid){

    memcpy(espacioUsuario+dirF,dato,cantBytes);

    log_info(logger, "PID:%d - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: FS",pid ,dirF,cantBytes);
}


void pedidoLectura_FS(uint32_t *cantBytes, uint32_t *dir_fisica, uint32_t *pid)
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

char* pedidoEscritura_FS(uint32_t *cantBytes, uint32_t *dir_fisica, uint32_t *pid){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_FileSystem,buffer);
    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));
    // DATO
    char* dato = malloc(*cantBytes);
    buffer_unpack(buffer,dato,*cantBytes);
    //PID
    buffer_unpack(buffer,pid,sizeof(uint32_t));

    buffer_destroy(buffer);
    return dato;
}


void enviarDato_FS(uint32_t cantBytes, char* dato){

    t_buffer* buffer =buffer_create();
    // CANTIDAD DE BYTES
    buffer_pack(buffer, &cantBytes, sizeof(uint32_t));
    // CADENA DE BYTES
    buffer_pack(buffer,dato, cantBytes);

    stream_send_buffer(conexion_con_FileSystem, FS_M_READ_OK, buffer);
    buffer_destroy(buffer);
}

void ok_FS(){
    stream_send_empty_buffer(conexion_con_FileSystem, FS_M_WRITE_OK);
}