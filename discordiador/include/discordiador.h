#ifndef DISCORDIADOR_H
#define DISCORDIADOR_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "servidor_discordiador.h"
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

typedef enum Estados{
	llegada,
	listo,
	trabajando,
	bloqueado_io,
	bloqueado_emergencia,
	finalizado
}Estados;

typedef enum Comandos{
	INICIAR_PATOTA_COM,
	INICIAR_PLANIFICACION_COM,
	PAUSAR_PLANIFICACION_COM,
	LISTAR_TRIPULANTE_COM,
	EXPULSAR_TRIPULANTE_COM,
	OBTENER_BITACORA_COM,
	FIN,
	TEST_MENSAJES
}Comandos;
 


Configuracion* configuracion;
t_list * new_list;
t_list * execute_list;
t_list * blocked_io;

char* puerto_discordiador;

void consola_miriam();
int socket_miriam;
void consola_miriam();
int socket_store;

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

void iniciar_patota(char * token); //?string? chequer commons
void consola();
void leer_tareas_txt();
void mandar_tareas_miriam();
#endif