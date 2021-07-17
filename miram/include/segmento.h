#ifndef SEGMENTO_H
#define SEGMENTO_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"


int contador_segmentos;
int contador_segmentos_libres;
int tamanio_memoria_segmentacion;
int compactaciones;
t_list *tabla_segmentos;
t_list *tabla_segmentos_pcb;
t_list *tabla_segmentos_tareas;
t_list *tabla_segmentos_tcb;
char* criterio_seleccion;

typedef enum
{
	LIBRE,
	OCUPADO,
} estado_segmento;

typedef struct
{
	int id;
	uint32_t dir_inicio;
	int desplazamiento;
	char *estado;
} Segmento;

typedef struct
{
	uint32_t PID;
	int segmento_nro;
	int tripulantes_activos;
} PCB_adm;

typedef struct
{
	uint32_t TID;
	uint32_t PID;
	int segmento_nro;
} TCB_adm;

typedef struct
{
	uint32_t PID;
	int segmento_nro;
} Tarea_adm;

Segmento* buscar_segmento_por_id(int id_segmento);
uint32_t buscar_patota_segmentacion(int PID);
uint32_t buscar_tripulante_segmentacion(int TCB_ID);
bool segmentos_orden (Segmento* segmento1, Segmento* segmento_mayor);
PCB_adm *buscar_pcb_adm_por_id(int PID);
pthread_mutex_t mutex_dump;
#endif