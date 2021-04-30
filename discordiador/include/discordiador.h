#ifndef DISCORDIADOR_H
#define DISCORDIADOR_H

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

int leer_config(void);
void atender_request_discordiador(uint32_t request_fd);

#endif