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

//Descripción: Pausa la planificacion, no hay cambios en colas, nadie ejecuta
//Hecho por: Emiliano
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

//Descripción: Finaliza tripulante que termino sus tareas
//Hecho por: Emiliano
void terminar_tripulante(Tripulante* un_tripulante)
{
	un_tripulante->tripulante_despierto = false;

	sem_wait(&mutexEXEC);
    sem_wait(&mutexEXIT);
	Tripulante *tripulante_auxiliar;

	for (int i = 0; list_size(execute_list) < i; i++)
	{
		tripulante_auxiliar = (Tripulante *) list_get(execute_list, i);
		if(tripulante_auxiliar->id_tripulante == un_tripulante->id_tripulante)
        {
            list_remove(execute_list, i);
      		list_add(exit_list, tripulante_auxiliar);
            break;
        }
      
	}
	sem_post(&mutexEXEC);
    sem_post(&mutexEXIT);
	sem_wait(&semaforoEXEC);
}

//Descripción: Desbloquea tripulante cuyo IO termino
//Hecho por: Emiliano

void desbloquear_tripulante(Tripulante* trip){
  int indice;
	sem_wait(&mutexBLOCK);
    sem_wait(&mutexREADY);

  for(int i =0; i<list_size(blocked_io);i++){
    Tripulante* trip_auxiliar = list_get(blocked_io,i);
    if(trip->id_tripulante == trip_auxiliar->id_tripulante){
      indice = i;
      break;
    }
  }

  if(indice!=NULL){
  //Se saca tripulante de cola de BLOCK y se pasa a cola de READY.

    list_add(ready_list, list_remove(blocked_io,indice));
	sem_post(&mutexBLOCK);
  sem_wait(&mutexREADY);

  miLogInfo("\nSe pasa el tripulante a la cola de READY\n");
}
}