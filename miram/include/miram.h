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

typedef struct Configuracion{
	int puerto;
    int tamanio_memoria;
    char* esquema_memoria;
	int tamanio_pagina;
    int tamanio_swap;
	char* path_swap;
	char* algoritmo_reemplazo;
	char* algoritmo_busqueda;
} Configuracion;




Configuracion* configuracion;
pthread_t threadSERVER;
pthread_t threadMEMORIA;
pthread_t threadMAPA;

char* puerto_miram;
int tamanio_memoria;

int leer_config(void);
void atender_request_miram(uint32_t request_fd);
void crear_grilla(void);
void* iniciar_servidor_miram();
void* iniciar_funciones_memoria();

//GUI
	NIVEL* nivel;

	int cols, rows;

char* proxima_tarea_tripulante(int tripulante_id);
char* buscar_posicion_tripulante(int tripulante_id);
#endif