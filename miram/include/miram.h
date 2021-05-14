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

typedef struct Configuracion{
	int puerto;
    int tamanio_memoria;
    char* esquema_memoria;
	int tamanio_pagina;
    int tamanio_swap;
	char* path_swap;
	char* algoritmo_reemplazo;
} Configuracion;

Configuracion* configuracion;
int leer_config(void) ;

void atender_request_miram(uint32_t request_fd);
void crear_grilla(void);

typedef struct{
	uint32_t PID;
	uint32_t Tareas;
} PCB;

typedef struct{
	uint32_t TID;
	char estado;
	uint32_t pos_X;
	uint32_t pos_y;
	uint32_t proxima_instruccion;
	uint32_t PCB;	
} TCB;

typedef struct {
	char* operacion;
	int parametro;
	int pos_x;
	int pos_y;
	int tiempo;
} tarea;

typedef struct {
	char* operacion;
	int parametro;
	int pos_x;
	int pos_y;
	int tiempo;
} segmento;

void* MEMORIA;

#endif