#include "planificador.h"

//Descripción: Despierta los tripulantes dormidos de la cola de ejecución
//Hecho por: Emiliano
void despertar_tripulantes()
{
	sem_wait(&mutexEXEC);
	Tripulante *tripulante;

	for (int i = 0; list_size(execute_list) < i; i++)
	{
		tripulante = (Tripulante *) list_get(execute_list, i);
		tripulante->tripulante_despierto = true;
        sem_post(&tripulante->semaforo_trip);
	}
	sem_post(&mutexEXEC);
}

//Descripción: Duerme los tripulantes despiertos de la cola de ejecución
//Hecho por: Emiliano
void dormir_tripulantes()
{
	sem_wait(&mutexEXEC);
	Tripulante *tripulante;

	for (int i = 0; list_size(execute_list) < i; i++)
	{
		tripulante = (Tripulante *) list_get(execute_list, i);
		tripulante->tripulante_despierto = false;
	}
	sem_post(&mutexEXEC);
}

//Descripción: Iniciar la planificación del discordiador
//Hecho por: Emiliano
void iniciar_planificacion()
{
	sem_post(&mutexNEW);
	sem_post(&mutexREADY);
	sem_post(&mutexBLOCK);
	sem_post(&mutexEXEC);
	sem_post(&mutexEXIT);

	despertar_tripulantes();

	planificacion_activada = true;

    while(1) {
        sem_wait(&semaforoEXEC);

        if (planificacion_activada) {
            planificar_tripulante();
        } else {
            break;
        }
    }

}

void pausar_planificacion()
{
	planificacion_activada = false;

    dormir_tripulantes();

	sem_wait(&mutexNEW);
	sem_wait(&mutexREADY);
	sem_wait(&mutexBLOCK);
	sem_wait(&mutexEXEC);
	sem_wait(&mutexEXIT);
}