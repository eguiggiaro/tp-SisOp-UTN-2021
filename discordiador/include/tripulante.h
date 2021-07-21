#ifndef TRIPULANTE_H
#define TRIPULANTE_H

//Shared libs#include "shared_utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "logger.h"
#include "paquete.h"
#include <pthread.h>
//#include <commons/string.h>
#include <commons/collections/list.h>
#include "discordiador.h"//?
#include "planificador.h"

void *inicializar_tripulante(Tripulante* );
void mover_en_x(Tripulante, int);//siempre es 1... pero a derecha(-1) o izqueirda(+1)
void mover_en_y(Tripulante, int);//siempre es 1... pero abajo(-1) o arriba(+1)
void comunicar_bitacora();//?
void mostrar_bitacora();//? me va a mandar tipo log y yo debo mostrar lindo... va, discordiador...
void atender_sabotaje();//?
void decodificar_tarea();
void execute();//no se me ocurre mejor nombre para cuando esta en EXEC...
void get_quantum_faltante();//para q el discordiador sepa cuanto le falta antes de sacarlo.
void planificar_con_FIFO(Tripulante*);

void comenzar_ejecucion(Tripulante*);

void ejecutar_proxima_tarea(Tripulante*);

void ejecutar_proxima_tarea_FIFO(Tripulante*); //lee el parametro nombre_tarea de la tarea asignada al tripulante
                                          //y envia al tripulante a realizar la tarea correspondiente
void generar_comida_FIFO(Tripulante*);
void generar_oxigeno_FIFO(Tripulante*);
void consumir_oxigeno_FIFO(Tripulante*);
void consumir_comida_FIFO(Tripulante*);
void generar_basura_FIFO(Tripulante*);
void descartar_basura_FIFO(Tripulante*);
void tarea_generica_FIFO(Tripulante*); //consiste solo en  llegar a la ubicación y esperar que pase el tiempo solicitado en el estado EXEC

void ejecutar_proxima_tarea_RR(Tripulante*);
void generar_comida_RR(Tripulante*);
void generar_oxigeno_RR(Tripulante*);
void consumir_oxigeno_RR(Tripulante*);
void consumir_comida_RR(Tripulante*);
void generar_basura_RR(Tripulante*);
void descartar_basura_RR(Tripulante*);
void tarea_generica_RR(Tripulante*);
void* mover_tripulante_RR(Tripulante*);

void bloquear_tripulante_io(Tripulante*);
void mover_tripulante(Tripulante*);
void consumir_ciclos_cpu(Tripulante* trip);
static void char_destroy_trip(char* self);
void liberar_tarea(Tarea* tarea);
#endif