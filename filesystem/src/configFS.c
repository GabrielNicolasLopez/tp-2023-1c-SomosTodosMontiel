#include "configFS.h"

t_config *config;
t_filesystem_config* configFS;

int socketMemoria;
int socketKernel;

// CONFIGURACION FS
t_filesystem_config *leerConfiguracion(t_config* config)
{
    t_filesystem_config* configuracion = malloc(sizeof(t_filesystem_config));

	configuracion->IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
	configuracion->PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    configuracion->PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    configuracion->PATH_SUPERBLOQUE = config_get_string_value(config, "PATH_SUPERBLOQUE");
    configuracion->PATH_BITMAP = config_get_string_value(config, "PATH_BITMAP");
    configuracion->PATH_BLOQUES = config_get_string_value(config, "PATH_BLOQUES");
    configuracion->PATH_FCB = config_get_string_value(config, "PATH_FCB");
    configuracion->RETARDO_ACCESO_BLOQUE = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

	return configuracion;
}

int crear_conexion_con_memoria()
{
    socketMemoria = crear_conexion(configFS->IP_MEMORIA, configFS->PUERTO_MEMORIA);
    if (socketMemoria == -1) {
        log_error(logger, "Error al intentar conectar con módulo Memoria. Finalizando FS...");
        return -1;
    }
    // Handshake
    stream_send_empty_buffer(socketMemoria, HANDSHAKE_filesystem);
    t_handshake memoriaResponse = stream_recv_header(socketMemoria);
    if (memoriaResponse != HANDSHAKE_ok_continue) {
        log_error(logger, "Error al hacer handshake con módulo Memoria");
        return -1;
    }
    log_info(logger, "FS se conectó con Memoria");
    return 0;
}

int crear_servidor_kernel()
{
    int server_fd_kernel = iniciar_servidor("127.0.0.1", configFS->PUERTO_ESCUCHA);
    if (server_fd_kernel == -1) {
        log_error(logger, "Error al intentar iniciar servidor");
        return -1;
    }
    log_info(logger, "Filesystem listo para recibir el socketKernel");

	socketKernel = esperar_cliente(server_fd_kernel);
    t_handshake handshake = stream_recv_header(socketKernel);
    if (handshake != HANDSHAKE_kernel) {
        log_error(logger, "Error al recibir handshake");
        return -1;
	}
    log_info(logger, "Se envia handshake ok continue a kernel");
    stream_send_empty_buffer(socketKernel, HANDSHAKE_ok_continue);

    log_info(logger, "FS se conectó con kernel");
    while (1)
    {
        
    }
}