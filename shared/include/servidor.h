
#ifndef SERVIDOR_H
#define SERVIDOR_H

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>
#include <paquete.h>
#include <buffer.h>

void levantar_servidor(void (*atender_request)(uint32_t), char* puerto);
void terminar_conexion(int socket_cliente);
int nueva_conexion(char* ip, char* puerto);
int esperar_conexion_cliente(int);
//dado un socket de entrada, devuelve la operacion recibida
int recibir_operacion(int socket);
//dado un socket de entrada, devuelve el buffer recibido
void* recibir_buffer(int socket);

typedef struct Request {
    op_code codigo_operacion;
	t_buffer* buffer_devolucion;
    int request_fd;
} Request;

#endif