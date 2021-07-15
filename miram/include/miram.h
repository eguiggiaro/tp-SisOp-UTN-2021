#ifndef MIRAM_H
#define MIRAM_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <paquete.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include "estructuras.h"
#include "servidor.h"

t_config *config;
typedef struct Configuracion
{
	int puerto;
	int tamanio_memoria;
	char *esquema_memoria;
	int tamanio_pagina;
	int tamanio_swap;
	char *path_swap;
	char *algoritmo_reemplazo;
	char *criterio_seleccion;
	char *mapa;
} Configuracion;

Configuracion *configuracion;
pthread_t threadSERVER;
pthread_t threadMEMORIA;
pthread_t threadMAPA;

char *puerto_miram;
int tamanio_memoria;
int tamanio_pagina;

int leer_config(void);
void atender_request_miram(Request *request);
void crear_grilla(void);
void *iniciar_servidor_miram();
void *iniciar_funciones_memoria();

//GUI
t_list *tabla_identificadores_grilla;
NIVEL *nivel;
int cols, rows;
// mejorar
char identificadores[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A'};
int proximo_identificador = 0;

char *proxima_tarea_tripulante(int tripulante_id);
char *buscar_posicion_tripulante(int tripulante_id);
char buscar_tripulante_grilla(int tripulante);
u_int32_t buscar_tripulante(int TCB_ID);
TCB* iniciar_tripulante(int patota_id);
int cambiar_cola_tripulante(int tripulante, char cola_destino);

pthread_mutex_t mutex_tripulantes;
pthread_mutex_t mutex_tareas;
pthread_mutex_t mutex_expulsion;
pthread_mutex_t mutex_patota;
pthread_mutex_t mutex_mover;

#endif