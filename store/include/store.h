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
#include <signal.h>

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
#include "servidor.h"
#include "logger.h"
#include "paquete.h"

typedef enum { 
	OXIGENO,
    COMIDA,
    BASURA, 
}tipoRecurso;

char* stringFromRecurso(tipoRecurso f);

typedef enum { 
	GENERAR_OXIGENO,
    GENERAR_COMIDA,
    GENERAR_BASURA,
    CONSUMIR_OXIGENO,
    CONSUMIR_COMIDA,
    DESCARTAR_BASURA, 
}tipoTarea;

typedef struct {
	int size;
	int block_count;
	t_list* blocks;
	char caracter_llenado;
	char md5[32];
} MetadataRecurso;

typedef struct {
	int size;
	int block_count;
	t_list* blocks;	
} MetadataBitacora;

typedef struct {
	uint32_t x;
	uint32_t y;
} t_pos;

typedef struct {
	t_pos* posicion;
	bool atendida;
} PosicionSabotaje;

t_list* posicionesSabotaje;

typedef struct {
	char* puntoMontaje;
	char* puerto;
	int tiempoSincro;
	char* posicionesSabotaje;
	uint32_t blockSizeDefault;
	uint32_t blocksQtyDefault;
	char* ipDiscordiador;
	char* puertoDiscordiador;
} Configuracion;

Configuracion* configuracion;
t_config* config;

int leerConfig();
void atender_request_store(Request *request);
void inicializarParametrosFS();
void inicializarStore();
void inicializarPosicionesSabotaje();
void inicializarSemaforos();
void syncPeriodico();
void atenderSabotaje();
void protocoloFsck();
void finalizarStore();
t_list* obtenerListaSabotaje(char*);


/**** SOPORTE ****/
static void char_destroy(char* self) {
    free(self);
}


pthread_mutex_t mutex_informartareas;
pthread_mutex_t mutex_informacionBitacora;
pthread_mutex_t mutex_obtenerBitacora;
pthread_mutex_t mutex_guardar_en_bitacora;

/*PARA DETENER PROCESO MIENTRAS SE ATIENDE EL SABOTAJE*/
pthread_mutex_t mutexEjecucionSabotaje;// = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condEjecucionSabotaje;// = PTHREAD_COND_INITIALIZER;
pthread_attr_t attrEjecucionSabotaje;
int esperaSabotaje;

#endif