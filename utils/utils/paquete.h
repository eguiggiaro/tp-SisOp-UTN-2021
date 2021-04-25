#ifndef PAQUETE_H_
#define PAQUETE_H_

#include "buffer.h"

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete* paquete, char* valor);
t_buffer* serializar_paquete(t_paquete* paquete);
void eliminar_paquete(t_paquete* paquete);

#endif