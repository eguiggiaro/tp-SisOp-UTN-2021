#ifndef STORE_H
#define STORE_H

//Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//Commons libs
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

//Shared libs
#include "shared_utils.h"

typedef struct Configuracion{
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
} Configuracion;

Configuracion* configuracion;

int leer_config(void);
void atender_request_store(uint32_t);

#endif