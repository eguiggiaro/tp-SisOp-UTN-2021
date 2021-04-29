#include "shared_utils.h"

char* mi_funcion_compartida(){
    return "Hice uso de la shared!";
}

void* metodo() {
t_log* logger = log_create("./cfg/discordiador.log", "Discordiador", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el discordiador desde utils! %s", mi_funcion_compartida());
    log_destroy(logger);
	return NULL;
}

int crear_conexion(char *ip, char* puerto)
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


void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void iniciar_servidor(void (*atender_request)(uint32_t), char* puerto)
{
	struct sockaddr_in direccionServidor;
	struct sockaddr_storage direccionRequest;
	socklen_t tamanioDireccion;
	int servidor_fd, puerto_app;

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

	if(listen(servidor_fd, 10) == -1){
		perror("fallo el listen");
	}

	printf("(Esperando conexiones en Direccion: %i, Puerto: %i ) \n",INADDR_ANY,puerto_app);
	  while(1){
		tamanioDireccion = sizeof(direccionRequest);
		pthread_t trequest;

		int request_fd;
		request_fd = accept(servidor_fd, (void*) &direccionRequest, &tamanioDireccion);

		//log_info(logger, "(Recibi request del puerto: %i)",request_fd);

		int thread_status = pthread_create(&trequest, NULL, (void*) atender_request,(void*) request_fd);
		if( thread_status != 0 ){
			log_error(logger, "Thread create returno %d", thread_status );
			log_error(logger, "Thread create returno %s", strerror( thread_status ) );
			} 
		else {
		  pthread_detach( trequest );
		}
	   }
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

t_log* iniciar_logger(char* logFile, char* proceso) {
	t_log *logger;
	logger = log_create(logFile,proceso,1,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(char* nombre){
	t_config* configuracion;
	configuracion = config_create(nombre);
	return configuracion;
}

void leer_consola(t_log* logger){
	char* leido;
	leido = readline(">");
	while(*leido != '\0')
	{
		log_info(logger,leido);
		free(leido);
		leido = readline(">");
	}
	free(leido);
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

	send(fd_socket, a_enviar, paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t),0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void loggear_lista_strings(t_list* lista, t_log* logger){

	if(list_size(lista)!=0){
		log_info(logger, "El contenido de la lista es el siguiente\n");
		for(int i = 0; i < list_size(lista); i++){
			log_info(logger, "\tEl elemento %d es : %s\n", i+1, (char*)list_get(lista,i));
		}
	}else{
		log_info(logger, "Lista vacia\n");
	}
}

t_buffer* serializar_lista_strings(t_list* lista){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) //cantidad de elementos de la lista
			+ peso_de_una_lista(lista); //peso de los elemetos de la lista

	uint32_t cantElementos,tamanio;
	char* char_auxiliar;
	cantElementos = list_size(lista);

	void* stream = malloc(buffer->size);
	int offset = 0;

	// memcpy
	memcpy(stream + offset, &cantElementos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 0; i < cantElementos; i++){
		char_auxiliar = list_get(lista,i);
		tamanio = strlen(char_auxiliar);

		memcpy(stream + offset, &tamanio, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, char_auxiliar, tamanio+1);
		offset += tamanio+1;
	}

 	buffer->stream = stream;

 	return buffer;
}

t_list* deserializar_lista_strings(t_buffer* buffer){

	uint32_t cantElementos, tamanio;
	char* char_auxiliar;
	t_list* lista = list_create();

 	void* stream = buffer->stream;

 	memcpy(&cantElementos, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	for(int i = 0; i < cantElementos; i++){
 		memcpy(&tamanio, stream, sizeof(uint32_t));
 		stream += sizeof(uint32_t);
 		char_auxiliar = malloc(tamanio+1);
 		memcpy(char_auxiliar, stream, tamanio+1);
 		stream += tamanio+1;

 		list_add(lista,char_auxiliar);
 	}
	return lista;
}

t_buffer* serializar_numero(uint32_t numero){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	memcpy(stream + offset, &numero, sizeof(uint32_t));
 	offset += sizeof(uint32_t);

 	buffer->stream = stream;

 	return buffer;
 }

uint32_t deserializar_numero(t_buffer* buffer){
 	uint32_t numero;

 	void* stream = buffer->stream;

 	memcpy(&numero, stream, sizeof(uint32_t));

	return numero;
 }


t_buffer* serializar_string(char* nombre){

 	t_buffer* buffer = malloc(sizeof(t_buffer));
 	buffer->size = sizeof(uint32_t) + strlen(nombre) + 1;
 	uint32_t tamanio = strlen(nombre);

 	void* stream = malloc(buffer->size);
 	int offset = 0;

 	memcpy(stream + offset, &tamanio, sizeof(uint32_t));
 	offset += sizeof(uint32_t);
 	memcpy(stream + offset, nombre, strlen(nombre)+1);
 	offset += strlen(nombre)+1;

 	buffer->stream = stream;

 	return buffer;
 }

char* deserializar_string(t_buffer* buffer){
 	uint32_t tamanio;
 	char* nombre;

 	void* stream = buffer->stream;

 	memcpy(&tamanio, stream, sizeof(uint32_t));
 	stream += sizeof(uint32_t);

 	nombre = malloc(tamanio);

 	memcpy(nombre, stream, tamanio);

	return nombre;
 }

int recibir_operacion(int fd_entrada)
{
	int cod_op;
	if(recv(fd_entrada, &cod_op, sizeof(op_code), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(fd_entrada);
		log_error(logger, "Error obteniendo codigo de operacion");
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

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete_con_operacion(op_code codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	crear_buffer(paquete);
	return paquete;
}

size_t peso_de_una_lista(t_list* lista){

	t_list* lista_auxiliar;
	char* char_auxiliar;
	size_t size = 0;

	lista_auxiliar = list_duplicate(lista);

	for(int i = 0 ; i < list_size(lista_auxiliar); i++){

		char_auxiliar = list_get(lista_auxiliar,i);
		size = size + (strlen(char_auxiliar)+1);

	}

	size = size + (sizeof(uint32_t)*list_size(lista));

	return size;
}

/* void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}*/