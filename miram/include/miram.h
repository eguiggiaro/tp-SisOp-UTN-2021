#ifndef MIRAM_H
#define MIRAM_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <paquete.h>

typedef struct Configuracion{
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
} Configuracion;

Configuracion* configuracion;
int leer_config(void) ;

#endif