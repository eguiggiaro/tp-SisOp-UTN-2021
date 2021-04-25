#ifndef SOCKETS_H_
#define SOCKETS_H_

#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<string.h>

int iniciar_servidor(t_log* logger);
int esperar_cliente(t_log* logger, int socket_servidor);
int crear_conexion(t_log* logger, char *ip, char* puerto);
void liberar_conexion(int* socket_cliente);

#endif