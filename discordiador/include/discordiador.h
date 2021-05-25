#ifndef DISCORDIADOR_H
#define DISCORDIADOR_H

//Shared libs
#include "shared_utils.h"
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/string.h>
#include<commons/collections/list.h>
//#include "servidor_discordiador.h"
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
	llegada,//?
	listo,
	trabajando,
	bloqueado_io,
	bloqueado_emergencia,
	finalizado//?
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

 
typedef struct Tripulante_disc{
	pthread_t * id_hilo;
	Estados estado_trip;
	int quantum;
} Tripulante_disc;

Configuracion* configuracion;
//Listas
t_list * new_list;
t_list * execute_list;
t_list * blocked_io;
void vaciar_listas();

//Metodos mensajes
int leer_config(void);
void iniciar_conexion_miram(char* ip_destino, char* puerto_destino);
void iniciar_conexion_store(char* ip_destino, char* puerto_destino);
int socket_miram;
int socket_store;
char* puerto_discordiador;



//Metodos Discordidor
void iniciar_patota(char* ); //?string? chequer commons
void consola();
char* leer_tareas_txt(char* direccion_txt);


//Metodos Test
void elegir_modulo();
void consola_miram();
void consola_store();
void iniciar_consola_miram();
void iniciar_consola_store();
bool opcion_invalida_miram(char*);
bool opcion_invalida_store(char*);
bool modulo_invalido(char*);
op_code convertir_codigo_operacion_miram(char*);
op_code convertir_codigo_operacion_store(char*);

#endif