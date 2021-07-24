#include "planificador.h"

//Descripción: Despierta los tripulantes dormidos de la cola de ejecución
//Hecho por: Emiliano
void despertar_tripulantes()
{
	pthread_mutex_lock(&mutexEXEC);
	Tripulante *tripulante;

	for (int i = 0; i < list_size(execute_list); i++)
	{
		tripulante = (Tripulante *) list_get(execute_list, i);
		tripulante->tripulante_despierto = true;
        pthread_mutex_unlock(&tripulante->semaforo_trip);
	}
	pthread_mutex_unlock(&mutexEXEC);
}

//Descripción: Duerme los tripulantes despiertos de la cola de ejecución
//Hecho por: Emiliano
void dormir_tripulantes()
{
	pthread_mutex_lock(&mutexEXEC);
	Tripulante *tripulante;

	for (int i = 0; i < list_size(execute_list); i++)
	{
		tripulante = (Tripulante *) list_get(execute_list, i);
		tripulante->tripulante_despierto = false;
		//pthread_mutex_lock(&tripulante->semaforo_trip);

	}
	pthread_mutex_unlock(&mutexEXEC);
}

//Descripción: Iniciar la planificación del discordiador
//Hecho por: Emiliano
void iniciar_planificacion()
{
	//pthread_mutex_unlock(&mutexNEW);
	pthread_mutex_unlock(&mutexREADY);
	pthread_mutex_unlock(&mutexBLOCK);
	pthread_mutex_unlock(&mutexEXEC);
	pthread_mutex_unlock(&mutexEXIT);

	despertar_tripulantes();

	planificacion_activada = true;

	miLogInfo("Se inicia la PLANIFICACION. \n");

    while(1) {
        sem_wait(&semaforoEXEC);
		sem_wait(&semaforoREADY);

        if (planificacion_activada) {
            planificar();
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

	miLogInfo("Se pausa la PLANIFICACION. \n");

    dormir_tripulantes();

	//pthread_mutex_lock(&mutexNEW);
	pthread_mutex_lock(&mutexREADY);
	pthread_mutex_lock(&mutexBLOCK);
	pthread_mutex_lock(&mutexEXEC);
	pthread_mutex_lock(&mutexEXIT);

	//pthread_exit(0);
}

//Descripción: Desbloquea tripulante cuyo IO termino
//Hecho por: Emiliano

void desbloquear_tripulante_io(Tripulante* trip){
  int indice;
  bool tripulante_encontrado = false;
  Tripulante* trip_auxiliar;
  pthread_mutex_lock(&mutexBLOCK);
  pthread_mutex_lock(&mutexREADY);
/*
  for(int i =0; i<list_size(blocked_io);i++){
    trip_auxiliar = list_get(blocked_io,i);
    if(trip->id_tripulante == trip_auxiliar->id_tripulante){
      indice = i;
	  tripulante_encontrado = true;
      break;
    }
  }
*/
  bool id_tripu(Tripulante* tripu){
	  return trip->id_tripulante == tripu->id_tripulante;
  }

  trip_auxiliar = list_remove_by_condition(blocked_io, id_tripu);

  if(trip_auxiliar!=NULL){
	list_add(ready_list, trip);
	trip->estado = listo;
	miLogInfo("Se desbloquea el tripulante: %d y pasa de BLOCK_IO a READY\n", trip->id_tripulante);
	informar_cambio_de_cola_miram(trip,"READY");
	sem_post(&semaforoREADY);
  }
  pthread_mutex_unlock(&mutexBLOCK);
  pthread_mutex_unlock(&mutexREADY);

  /*if(tripulante_encontrado){
  //Se saca tripulante de cola de BLOCK y se pasa a cola de READY.
    trip_auxiliar = list_remove(blocked_io,indice);
    list_add(ready_list, trip_auxiliar);
	trip_auxiliar->estado = listo;
	pthread_mutex_unlock(&mutexBLOCK);
    pthread_mutex_unlock(&mutexREADY);
    sem_post(&semaforoREADY);
    miLogInfo("Se desbloquea el tripulante: %d y pasa de BLOCK_IO a READY\n", trip->id_tripulante);
	//aviso cambio de cola a MIRAM
    informar_cambio_de_cola_miram(trip_auxiliar,"READY");
  }
  else{
	  pthread_mutex_unlock(&mutexBLOCK);
      pthread_mutex_unlock(&mutexREADY);
  }*/
}