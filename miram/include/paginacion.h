#ifndef PAGINACION_H
#define PAGINACION_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"


int contador_frames;
int contador_frames_libres;
int contador_paginas;
int tamanio_pagina_paginacion;
t_list *tabla_frames;
t_list *tabla_pcbs;
t_list *tabla_tareas;
t_list *tabla_tcbs;


typedef enum
{
	LIBRE,
	OCUPADO,
} estado_frame;

typedef struct Frame
{
	int id_frame;
	uint32_t dir_inicio;
	char *estado;
	int id_pagina;
} Frame;

typedef struct Pagina
{
	int id_pagina;
	int id_frame;
	char *estado;
	t_list* contenido;
} Pagina;

typedef struct
{
	uint32_t PID;
	t_list* tabla_paginas;
	int pagina_inicio_pcb;
	int cantidad_paginas_pcb;
	int pagina_inicio_tareas;
	int cantidad_paginas_tareas;
	int offset_tareas;
	int tamanio_tareas;
	t_list* tabla_TCB_admin;
	int tripulantes_iniciados;
} PCB_adm;

typedef struct
{
	uint32_t TID;
	int pagina_inicio;
	int offset;
	int cantidad_paginas;
} TCB_adm;

char *buscar_tareas(int patota_id);
TCB *buscar_tcb_por_id(int tripulante_id);
pthread_mutex_t mutex_dump;
#endif