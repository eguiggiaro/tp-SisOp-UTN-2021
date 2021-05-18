#ifndef STORE_H
#define STORE_H

//Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

//Commons libs
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

//Shared libs
#include "shared_utils.h"


typedef struct {
	uint32_t x;
	uint32_t y;
}t_pos;

typedef struct {
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
	//char** posicionesSabotaje;
	uint32_t blockSizeDefault;
	uint32_t blocksQtyDefault;
} Configuracion;

Configuracion* configuracion;

int leer_config(void);
void atender_request_store(uint32_t);
void inicializar_store();

#endif