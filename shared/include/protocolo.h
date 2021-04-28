#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "sockets.h"
#include "buffer.h"
#include "paquete.h"
#include <commons/collections/list.h>
#include <commons/string.h>

void enviar_mensaje(char*, int);
void enviar_paquete(t_paquete*, int);

int recibir_operacion(int);
t_buffer* recibir_buffer(int);
char* recibir_mensaje(t_log*, int);
t_list* recibir_paquete(int);
#endif
