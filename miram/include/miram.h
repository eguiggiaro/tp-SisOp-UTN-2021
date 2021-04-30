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
    int ip_mi_ram_hq;
    int puerto_mi_ram_hq;
} Configuracion;

Configuracion* configuracion;
int leer_config(void) ;

void atender_request_miram(uint32_t request_fd);

#endif