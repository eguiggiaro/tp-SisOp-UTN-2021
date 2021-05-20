#ifndef TRIPULANTE_H
#define TRIPULANTE_H

//Shared libs#include "shared_utils.h"
#include <stdio.h>
#include <stdbool.h>
#include "logger.h"
#include "paquete.h"
#include <pthread.h>
//#include <commons/string.h>
#include <commons/collections/list.h>
#include "discordiador.h"?



typedef struct Tarea{
    //should i use ponters for all right?
    char* tarea_completa;//? - chequear la libreria de strings... 
    char* tarea;//chequear la libreria de strings... 
    int pos_x;
    int pos_y;
    int tiempo;
} Tarea;

typedef struct Tripulante{
    //should i use ponters for all right?
    int id_patota;
    int id_tripulante;
    int pos_x;
    int pos_y;
    Estados estado;//deberia saber yo mi estado for sakes de semaforos? de ser si, q sea un enum...
    char* tarea_actual;//chequear la libreria de strings... 
    
} Tripulante;

void *iniciar_tripulante(void * );
void mover_en_x(Tripulante, int);//siempre es 1... pero a derecha(-1) o izqueirda(+1)
void mover_en_y(Tripulante, int);//siempre es 1... pero abajo(-1) o arriba(+1)
void comunicar_bitacora();//?
void mostrar_bitacora();//? me va a mandar tipo log y yo debo mostrar lindo... va, discordiador...
void atender_sabotaje();//?
void decodificar_tarea();
void execute();//no se me ocurre mejor nombre para cuando esta en EXEC...
void get_quantum_faltante();//para q el discordiador sepa cuanto le falta antes de sacarlo.







#endif