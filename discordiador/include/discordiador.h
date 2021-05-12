#ifndef DISCORDIADOR_H
#define DISCORDIADOR_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "logger.h"
#include "paquete.h"

typedef struct Configuracion{
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
	char* ip_mi_ram_hq;
	int puerto_mi_ram_hq;
	char* ip_i_mongo_store;
	int puerto_i_mongo_store;
	int grado_multitarea;
	char* algoritmo;
	int quantum;
	int duracion_sabotaje;
	int retardo_ciclo_cpu;
} Configuracion;

Configuracion* configuracion;

int leer_config(void);
void elegir_modulo();
void iniciar_consola_miram();
void iniciar_consola_store();
bool opcion_invalida_miram(char*);
bool opcion_invalida_store(char*);
bool modulo_invalido(char*);
op_code convertir_codigo_operacion_miram(char*);
op_code convertir_codigo_operacion_store(char*);
void iniciar_conexion_miram(char* ip_destino, char* puerto_destino);
void iniciar_conexion_store(char* ip_destino, char* puerto_destino);
#endif