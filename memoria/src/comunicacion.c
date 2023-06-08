#include <memoria.h>
#include <funciones.h>


void iniciar_servidor_con_karnel()
{
    int server_fd =iniciar_servidor(IPPORT_EFSSERVER,configuracionMemoria -> PUERTO_ESCUCHA_KERNEL);
    log_info(logger, "Servidor listo para recibir al kernel");

    socketKernel= esperar_cliente(server_fd,logger);
    char *mensaje = recibirMensaje(socketKernel);

	log_info(logger, "Mensaje de confirmacion del Kernel: %s\n", mensaje);
   
    t_tipoInstruccion instruccion;

    while (1)
    {
    log_info(logger, "Estoy esperando paquete, soy memoria\n");
		t_paquete *paquete = recibirPaquete(socketKernel);

        

        switch (paquete->codigo_operacion)
        {
        case CREATE_SEGMENT:
            log_info(logger, "Mi cod de op es: %d", paquete->codigo_operacion);
            /*
            -aca recibo el tamanio del segmento, la id, y la base 
            -creo el segemnto y lo meto en la lista         
            -devuelvo la lista actualizada, 
            */
            break;

        case DELETE_SEGMENT:

            log_info(logger, "Mi cod de op es: %d", paquete->codigo_operacion);

            //recibo el segemto a eliminar y devuelvo la lista nueva con el segemto elimado     
            
            
            break;


        //falta los casos de cracion de estructuras basicas pero nose que mandan o como hacemos eso 

        default:
            break;
        }
        free(paquete);
    }
    
}