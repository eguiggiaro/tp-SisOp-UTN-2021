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
#include<semaphore.h>

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
	finalizado,//?
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

	
Comandos find_enum_consola(char *sval);

typedef struct Tarea{
	char* nombre_tarea;
	char* parametros;
	char* pos_x;
	char* pos_y;
	int tiempo;
}Tarea;
 
typedef struct Tripulante_disc{
	pthread_t * id_hilo;
	Estados estado_trip;
	int quantum;
} Tripulante_disc;

typedef struct Tripulante{
    //should i use ponters for all right?
    int id_patota;
    int id_tripulante;
    int pos_x;
    int pos_y;
    Estados estado;//deberia saber yo mi estado for sakes de semaforos? de ser si, q sea un enum...
    //char* tarea_actual;//chequear la libreria de strings... 
	Tarea* tarea_actual;
	pthread_t* id_hilo;
	sem_t semaforo_trip; //este semaforo indica si el tripulante puede ejecutar
	bool tripulante_despierto; //indica cuando está despierto el tripulante, en caso de estar dormido no puede ejecutar
    bool completo_tareas;
} Tripulante;

Configuracion* configuracion;
//Listas
t_list * new_list;
t_list * execute_list;
t_list * blocked_io;
t_list* exit_list;
t_list* ready_list;

//Semaforos
sem_t mutexNEW;
sem_t mutexEXEC;
sem_t semaforoEXEC;
sem_t semaforoREADY;
sem_t mutexBLOCK;
sem_t mutexEXIT;
sem_t mutexREADY;

//Metodos mensajes
int leer_config(void);
void iniciar_conexion_miram(char* ip_destino, char* puerto_destino);
void iniciar_conexion_store(char* ip_destino, char* puerto_destino);
int socket_miram;
int socket_store;
char* puerto_discordiador;

int aux_id_tripulante;

bool planificacion_activada;


//Metodos Discordidor
void iniciar_patota(char* ); //?string? chequer commons
void consola();
char* leer_tareas_txt(char* direccion_txt);
void mandar_tareas_miram(char *);
int enviar_tareas_miram(char* direccion_txt);
void tripulante_listo(Tripulante*);
Tarea* obtener_tarea(char* tarea_string, Tarea* nueva_tarea); //dado un string con los parametros de tarea, genera la estructura Tarea
void iniciar_planificacion();
void pausar_planificacion();
void finalizar_tripulante(Tripulante* trip); //Tripulante pasa de EXEC a EXIT, se liberan sus recursos
bool tarea_informada(int id_tripulante, char* nombre_tarea, char* parametro);

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