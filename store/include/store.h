#ifndef STORE_H
#define STORE_H

//Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/mman.h>

//Commons libs
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/memory.h>
#include <commons/collections/list.h>

//Shared libs
#include "shared_utils.h"

typedef enum { 
	OXIGENO,
    COMIDA,
    BASURA, 
}tipoRecurso;

typedef enum { 
	GENERAR_OXIGENO,
    GENERAR_COMIDA,
    GENERAR_BASURA,
    CONSUMIR_OXIGENO,
    CONSUMIR_COMIDA,
    DESECHAR_BASURA, 
}tipoTarea;

typedef struct {
	uint32_t x;
	uint32_t y;
}t_pos;

typedef struct {
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
	char* posicionesSabotaje;
	uint32_t blockSizeDefault;
	uint32_t blocksQtyDefault;
} Configuracion;

Configuracion* configuracion;

int leerConfig(void);
void atender_request_store(uint32_t);
void inicializarParametrosFS(void);
void inicializarStore(void);
t_list* obtenerListaSabotaje(char*);

#endif