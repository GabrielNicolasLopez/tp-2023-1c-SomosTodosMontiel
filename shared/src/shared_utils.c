#include "shared_utils.h"

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

char *mi_funcion_compartida()
{
	return "Hice uso de la shared!";
}

// Esto es del cliente

void *serializar_paquete(t_paquete *paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0; // esto es el offset que esta en la guia de serializacion

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char *puerto, t_log *logger)
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

void enviar_mensaje(char *mensaje, int socket_cliente)
{

	// esto es lo de crear_paquete()
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	// todo esto es lo mismo que enviar_paquete()
	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(void)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

// Esto es del servidor

t_log *logger;
t_log *loggerMinimo;

int iniciar_servidor(char *IP, char *PUERTO, t_log *logger)
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

int esperar_cliente(int socket_servidor, t_log *logger)
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

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{

		return -1;
	}
}

void *recibir_buffer(uint32_t *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list *recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

int size_char_array(char **array)
{
	int i = 0;

	while (array[i] != NULL)
	{
		i++;
	}
	return i;
}

int enviarMensaje(int socket, char *msj){
	size_t size_stream;
	void *stream = serializarMensaje(msj, &size_stream);
	return enviarStream(socket, stream, size_stream);
}

void *serializarMensaje(char *msj, size_t *size_stream){

	*size_stream = strlen(msj) + 1;

	void *stream = malloc(sizeof(*size_stream) + *size_stream);

	memcpy(stream, size_stream, sizeof(*size_stream));
	memcpy(stream + sizeof(*size_stream), msj, *size_stream);

	*size_stream += sizeof(*size_stream);

	return stream;
}

int enviarStream(int socket, void *stream, size_t stream_size){

	if (send(socket, stream, stream_size, 0) == -1)
	{
		free(stream);
		return 0;
	}

	free(stream);
	return 1;
}


// Serializar
/*
void serializarPCB(int socket, t_pcb *pcb, t_tipoMensaje tipoMensaje)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint32_t) * 5 + calcularSizeInfo(pcb->informacion)
	+ list_size(pcb->informacion->segmentos) * sizeof(uint32_t)
	+ list_size(pcb->tablaSegmentos) * sizeof(t_tabla_segmentos)
	+ sizeof(int) + sizeof(t_registros);

	void *stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &pcb->id, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pcb->program_counter, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &pcb->socket, sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &pcb->registros, sizeof(t_registros));
	offset += sizeof(t_registros);

	memcpy(stream + offset, &(pcb->informacion->instrucciones->elements_count), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(pcb->informacion->segmentos->elements_count), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(pcb->tablaSegmentos->elements_count), sizeof(uint32_t));
	offset += sizeof(uint32_t);


	int i = 0, j = 0 , n = 0;

	while (i < list_size(pcb->informacion->instrucciones))
	{
		t_instruccion* instrucccion = list_get(pcb->informacion->instrucciones, i);
		memcpy(stream + offset,&instrucccion->instCode, sizeof(t_instCode));
		offset += sizeof(t_instCode);
		memcpy(stream + offset,&instrucccion->paramInt, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset,&instrucccion->sizeParamIO, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		//printf("\ndispositivo%s\n" ,instrucccion->paramIO);
		memcpy(stream + offset,instrucccion->paramIO,instrucccion->sizeParamIO);
		offset += instrucccion->sizeParamIO;
		memcpy(stream + offset,&instrucccion->paramReg[0], sizeof(t_registro));
		offset += sizeof(t_registro);
		memcpy(stream + offset,&instrucccion->paramReg[1], sizeof(t_registro));
		offset += sizeof(t_registro);
		i++;
		//printf(PRINT_COLOR_MAGENTA "Estoy serializando las instruccion %d" PRINT_COLOR_RESET "\n", i);
	}

	while (j < list_size(pcb->informacion->segmentos))
	{
		uint32_t segmento = list_get(pcb->informacion->segmentos, j);
		memcpy(stream + offset, &segmento, sizeof(uint32_t));

		offset += sizeof(uint32_t);

		j++;
		//printf(PRINT_COLOR_YELLOW "Estoy serializando el segmento: %d" PRINT_COLOR_RESET "\n", j);

	}


	while (n < list_size(pcb->tablaSegmentos))
	{

		memcpy(stream + offset, list_get(pcb->tablaSegmentos, n), sizeof(t_tabla_segmentos));
		offset += sizeof(t_tabla_segmentos);
		n++;
		//printf(PRINT_COLOR_YELLOW "Estoy serializando el segmento: %d" PRINT_COLOR_RESET "\n", n);
	}

	buffer->stream = stream;

	crearPaquete(buffer, tipoMensaje, socket);

	//free(buffer);
}*/

/*int calcularSizeInfo(t_informacion* info){
	int total = 0;
	for (int i = 0 ; i < list_size(info->instrucciones); i++){

		t_instruccion* instruccion = list_get(info->instrucciones,i);
		total += instruccion->sizeParamIO;
		total += sizeof(uint32_t) * 2;
		total += sizeof(t_instCode);
		total += sizeof(t_registro) * 2;
	}

	return total;
}

void crearPaquete(t_buffer *buffer, t_tipoMensaje op, int unSocket)
{
	t_paqueteActual *paquete = malloc(sizeof(t_paqueteActual));
	paquete->codigo_operacion = (uint8_t)op;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));

	int offset = 0;
	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	send(unSocket, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

// Deserializar
t_paqueteActual *recibirPaquete(int socket)
{
	t_paqueteActual *paquete = malloc(sizeof(t_paqueteActual));
	paquete->buffer = malloc(sizeof(t_buffer));

	// Primero recibimos el codigo de operacion
	int rec = recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), MSG_WAITALL);
	if (rec <= 0)
	{
		return NULL;
	}

	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

	return paquete;
}

t_pcb *deserializoPCB(t_buffer *buffer)
{
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->informacion = malloc(sizeof(t_informacion));
	pcb->tablaSegmentos = malloc(sizeof(t_tabla_segmentos));

	void *stream = buffer->stream;

	// Deserializamos los campos que tenemos en el buffer
	memcpy(&(pcb->id), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->program_counter), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->socket), stream, sizeof(int));
	stream += sizeof(int);

	memcpy(&(pcb->registros), stream, sizeof(t_registros));
	stream += sizeof(t_registros);

	memcpy(&(pcb->informacion->instrucciones_size), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->informacion->segmentos_size), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&(pcb->segmentos_size), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	pcb->informacion->instrucciones = list_create();
	t_instruccion *instruccion;

	pcb->informacion->segmentos = list_create();
	uint32_t segmento;

	pcb->tablaSegmentos = list_create();
	t_tabla_segmentos* tableSegmentos;

	int k = 0, l = 0 , m = 0;

	while (k < (pcb->informacion->instrucciones_size))
	{
		instruccion = malloc(sizeof(t_instruccion));
		memcpy(&instruccion->instCode,stream, sizeof(t_instCode));
		stream += sizeof(t_instCode);
		memcpy(&instruccion->paramInt,stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		memcpy(&instruccion->sizeParamIO, stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		instruccion->paramIO = malloc(instruccion->sizeParamIO);
		memcpy(instruccion->paramIO,stream, instruccion->sizeParamIO);
		stream += instruccion->sizeParamIO;
		memcpy(&instruccion->paramReg[0],stream, sizeof(t_registro));
		stream += sizeof(t_registro);
		memcpy(&instruccion->paramReg[1],stream , sizeof(t_registro));
		stream += sizeof(t_registro);

		list_add(pcb->informacion->instrucciones, instruccion);
		k++;
	}

	while (l < (pcb->informacion->segmentos_size))
	{
		memcpy(&segmento, stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		list_add(pcb->informacion->segmentos, segmento);
		l++;
	}

	while (m < (pcb->segmentos_size))
	{
		tableSegmentos = malloc(sizeof(t_tabla_segmentos));
		memcpy(tableSegmentos, stream, sizeof(t_tabla_segmentos));
		stream += sizeof(t_tabla_segmentos);
		list_add(pcb->tablaSegmentos, tableSegmentos);
		m++;
	}

	//free(instruccion);
	//free(tableSegmentos);

	return pcb;
}

// Imprimir instrucciones y segmentos
void imprimirInstruccionesYSegmentos(t_informacion informacion)
{
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));

	// mostrar instrucciones
	printf("Instrucciones:");
	for (int i = 0; i < informacion.instrucciones_size; ++i)
	{

		instruccion = list_get(informacion.instrucciones, i);
		//printf("\n codigo %d", instruccion->instCode);
		//printf("\n dispositivo io %s", instruccion->paramIO);

		printf("\ninstCode: %d, Num: %d, RegCPU[0]: %d,RegCPU[1] %d , dispIO: %s\n",
			   instruccion->instCode, instruccion->paramInt, instruccion->paramReg[0], instruccion->paramReg[1],instruccion->paramIO);
	}

	// mostrar segmentos
	printf("\n\nimprimo cantidad de segmentos: %d\n",informacion.segmentos_size);
	for (int i = 0; i < informacion.segmentos_size; ++i)
	{
		uint32_t segmento = list_get(informacion.segmentos, i);

		printf("Segmento n°%d: %d\n",i , segmento);
	}
}
*/

