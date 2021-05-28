#include "servidor.h"

void levantar_servidor(void (*atender_request)(uint32_t), char* puerto)
{
	struct sockaddr_in direccionServidor;
	struct sockaddr_storage direccionRequest;
	socklen_t tamanioDireccion;
	int servidor_fd, puerto_app;
	op_code codigo_operacion;
	t_buffer* buffer_devolucion;
	Request* request;

	puerto_app = atoi(puerto);
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto_app);

	servidor_fd = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor_fd,SOL_SOCKET,SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor_fd, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("fallo el bind");
	}

	while (1)
 	{

		if(listen(servidor_fd, 10) == -1){
			perror("fallo el listen");
		}

		printf("(Esperando conexiones en Direccion: %i, Puerto: %i ) \n",INADDR_ANY,puerto_app);

		int request_fd;
		request_fd = accept(servidor_fd, (void*) &direccionRequest, &tamanioDireccion);
		
		printf("Se conectó un cliente!");

		while(1){

			codigo_operacion = recibir_operacion(request_fd);

			if (codigo_operacion == -1)
			{
				printf("El cliente se desconectó");
				break;
			}

			buffer_devolucion = recibir_buffer(request_fd);

			request = malloc(sizeof(Request));
			request->codigo_operacion = codigo_operacion;
			request->buffer_devolucion = buffer_devolucion;
			request->request_fd = request_fd;

			tamanioDireccion = sizeof(direccionRequest);
			pthread_t trequest;


			//log_info(logger, "(Recibi request del puerto: %i)",request_fd);

			int thread_status = pthread_create(&trequest, NULL, (void*) atender_request,(void*) request);
			if( thread_status != 0 ){
				printf("Thread create returno");
				printf("Thread create returno");
				} 
			else {
			pthread_detach( trequest );
			}

		}
	}
}

int nueva_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		printf("ip: %s",ip);
		printf("puerto: %s",puerto);
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}


void terminar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


int esperar_conexion_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int fd_entrada)
{
	
	int cod_op;
	if(recv(fd_entrada, &cod_op, sizeof(op_code), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(fd_entrada);
		printf("Error obteniendo codigo de operacion");
		return -1;
	}
}
void* recibir_buffer(int socket)
{
	t_buffer* buffer;
	buffer = malloc(sizeof(t_buffer));

	recv(socket, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);
	buffer->stream = malloc(buffer->size);
	recv(socket, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

void enviar_paquete(t_paquete* paquete, int fd_socket){

	void* a_enviar = malloc(paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_code));
	offset += sizeof(op_code);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	int retorno = send(fd_socket, a_enviar, paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t),0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}