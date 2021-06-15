#include "planificador.h"

//Descripción: Despierta los tripulantes dormidos
//Hecho por: Emiliano
void despertar_tripulantes()
{
	sem_wait(&mutexEXEC);
	Tripulante *tripulante;

	for (int i = 0; list_size(execute_list) < i; i++)
	{
		tripulante = (Tripulante *) list_get(execute_list, i);
		tripulante->tripulante_despierto = true;
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
}

void pausar_planificacion()
{
	planificacion_activada = false;
}