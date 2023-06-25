#include "shared_utils.h"

char *razonFinConsola[] = {"INSTRUCCIONES", "FIN", "OUT_OF_MEMORY", "SIGNAL", "WAIT"};
char *nombresCodigoOperaciones[] = {"MENSAJE", "PAQUETE", "NEW"};
char *nombresInstrucciones[] = {"F_READ", "F_WRITE", "SET", "MOV_IN", "MOV_OUT", "F_TRUNCATE", "F_SEEK", "CREATE_SEGMENT", "IO", "WAIT", "SIGNAL", "F_OPEN", "F_CLOSE", "DELETE_SEGMENT", "YIELD", "EXIT"};
    
char *nombresRegistros[] = 
{
	// 4 bytes
	"AX",
    "BX",
    "CX",
    "DX",
    // 8 bytes
    "EAX",
    "EBX",
    "ECX",
    "EDX",
    // 16 bytes
    "RAX",
    "RBX",
    "RCX",
    "RDX"
};

t_log *logger;
t_log *loggerMinimo;

int crear_conexion(char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// getaddrinfo(): es una llamada al sistema que devuelve la informacion de red sobre la IP y puerto que le pasemos, en este caso el servidor

	getaddrinfo(ip, puerto, &hints, &server_info);
	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	// connect():
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		//log_error(logger, "Error al conectarse!");
		return socket_cliente = -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


int iniciar_servidor(char *IP, char *PUERTO)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;		 /* ai_family = Protocol family for socket.  */
	hints.ai_socktype = SOCK_STREAM; /* ai_socktype =	Socket type.  */
	hints.ai_flags = AI_PASSIVE;	 /*ai_flags= Input flags.  */

	// char* ip = config_get_string_value(archivoConfig, "IP");
	// char* puerto = config_get_string_value(archivoConfig, "PUERTO");
	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	// Asociamos el socket a un puerto
	// bind() lo que hace es tomar el socket que creamos con anterioridad y pegarlo con pegamento industrial al puerto que le digamos

	/* ai_addr: Socket address for socket.  */
	/* ai_addrlen: Length of socket address.  */
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			printf("connection_error_crear_socket");
			continue;
		}
		int yes = 1;
		setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) != 0)
		{
			printf("Error en el bind");
			close(socket_servidor);
			continue;
		}
		break;
	}
	// Escuchamos las conexiones entrantes
	// listen() : toma el socket del servidor creado y lo marca en el sistema como un socket cuya unica responsabilidad es notificar cuando
	// un nuevo cliente esta intentando conectarse

	listen(socket_servidor, SOMAXCONN); // SOMAXCONN: es la cantidad maxima de conexiones vivas que admite el sistema operativo
	freeaddrinfo(servinfo);
	// log_trace(logger, "Listo para escuchar a mi cliente");
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente;

	// accept() es bloqueante, significa que el proceso servidor se quedara bloqueado en accept hasta que llegue un cliente

	struct sockaddr_in direccion_Cliente;
	socklen_t tam_Direccion = sizeof(struct sockaddr_in);

	socket_cliente = accept(socket_servidor, (void *)&direccion_Cliente, &tam_Direccion);

	// Una vez que el cliente aceptado, accept retorna un nuevo socket(file descriptor) que representa la conexion BIDIRECCIONAL entre el servidor y el cliente
	// La comunicacion entre el cliente y el servidor se hace entre el socket que realizo connect(lado del cliente) hacia el servidor, y el socket que fue devuelto por el accept

	log_info(logger, "Se conecto un cliente! socket: %d", socket_cliente);

	return socket_cliente;
}