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
	int ip_mi_ram_hq;
	int puerto_mi_ram_hq;
	int ip_i_mongo_store;
	int puerto_i_mongo_store;
	int grado_multitarea;
	int algoritmo;
	int quantum;
	int duracion_sabotaje;
	int retardo_ciclo_cpu;
} Configuracion;

Configuracion* configuracion;

int leer_config(void);


#endif