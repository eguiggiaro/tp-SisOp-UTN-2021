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
int contador_LRU;
int contador_espacios_SWAP;
FILE* SWAP;
void* bitmap;
t_list *tabla_frames;
t_list *tabla_pcbs;
t_list *tabla_tareas;
t_list *tabla_tcbs;
t_list *tabla_elementos_SWAP;


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
	bool en_memoria;
	int LRU;
} Pagina;

typedef struct Elemento_SWAP
{
	int id_pagina;
	int posicion_SWAP;
} Elemento_SWAP;


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
Frame *buscar_frame_por_pagina(int id_pagina);
PCB_adm* buscar_patota_tripulante(int tripulante_id);
Pagina *buscar_pagina_por_id(int id_pagina);
Frame* traer_de_SWAP(int id_pagina);
void* llevar_a_swap(Frame* un_frame, Pagina* una_pagina);
Frame *hacer_lugar_memoria();

pthread_mutex_t mutex_dump;
#endif