#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

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
#include <paquete.h>
#include <buffer.h>


void leer_consola(t_log* logger);
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
//copia los elementos de un array (char**) a un t_list* para mejor manipulacion de los mismos
void agregar_array_a_lista(t_list* lista, char** array);
//calcula tamano ocupado por una t_list* en memoria
size_t peso_de_una_lista(t_list* lista);
#endif