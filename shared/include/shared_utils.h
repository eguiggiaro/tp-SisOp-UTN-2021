//#ifndef SHARED_UTILS_H
//#define SHARED_UTILS_H
#ifndef SOCKET_H_
#define SOCKET_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<stdbool.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>

typedef enum
{
	MENSAJE,
	PAQUETE,
	OK = 20,
	FAIL = 21,
}op_code;

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_log* logger;

char* mi_funcion_compartida();
void* metodo();
void iniciar_servidor(void (*atender_request)(uint32_t), char* puerto);
int esperar_cliente(int);
t_log* iniciar_logger(char* logFile, char* proceso);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);
t_config* leer_config(char* nombre);
void leer_consola(t_log* logger);
t_paquete* armar_paquete();
void loggear_lista_strings(t_list* lista, t_log* logger);
//agrega una lista elementos de tipo string (char*) contenidos dentro de un buffer
t_buffer* serializar_lista_strings(t_list* lista);
//devuelve una lista elementos de tipo string (char*) contenidos dentro de un buffer
t_list* deserializar_lista_strings(t_buffer* buffer);
//agrega un unico elemento de tipo int contenido dentro de un buffer
t_buffer* serializar_numero(uint32_t numero);
//devuelve un unico elemento de tipo int contenido dentro de un buffer
uint32_t deserializar_numero(t_buffer* buffer);
//agrega un unico elemento de tipo string (char*) a un buffer
t_buffer* serializar_string(char* nombre);
//devuelve un unico elemento de tipo string (char*) contenido dentro de un buffer
char* deserializar_string(t_buffer* buffer);
//dado un socket de entrada, devuelve la operacion recibida
int recibir_operacion(int socket);
//dado un socket de entrada, devuelve el buffer recibido
void* recibir_buffer(int socket);
//copia los elementos de un array (char**) a un t_list* para mejor manipulacion de los mismos
void agregar_array_a_lista(t_list* lista, char** array);
void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete_con_operacion(op_code codigo_operacion);
//calcula tamano ocupado por una t_list* en memoria
size_t peso_de_una_lista(t_list* lista);
void enviar_paquete(t_paquete* paquete, int fd_socket);
#endif