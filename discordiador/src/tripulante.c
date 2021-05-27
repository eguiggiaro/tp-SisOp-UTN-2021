#include "tripulante.h"

void *inicializar_tripulante(void* tripulante_dato){
  aux_id_tripulante++; 

  Tripulante *tripulante = (Tripulante *)tripulante_dato;

  tripulante->id_tripulante = aux_id_tripulante;
  tripulante->pos_x = 1;
  tripulante->pos_y = 1;
  tripulante->tarea_actual = "GENERAR_COMIDA 2;1;0;4";

  miLogInfo("\nSe pasa el tripulante a la cola de NEW\n");

  sem_wait(&mutexNEW);
  list_add(new_list, tripulante);
  sem_post(&mutexNEW);

  //Una vez realizadas las tareas de preparacion, aviso a Discordiador que estoy listo
  tripulante_listo(tripulante);


  //1. Aviso a MIRAM que deseo iniciar, indicando a que patota pertenezco.
  //  t_paquete* paquete = crear_paquete(INICIAR_TRIPULANTE);
  //  t_buffer* buffer;
   
  //  char* id_patota = string_itoa(tripulante->id_patota);
  //  t_list* lista_mensajes = list_create();
  //  list_add(lista_mensajes,id_patota);
  //  buffer = serializar_lista_strings(lista_mensajes);
  //  paquete ->buffer = buffer;
  
  //  enviar_paquete(paquete, socket_miram);

  //  //recibe respuesta de destino
	// op_code codigo_operacion = recibir_operacion(socket);
	// if (codigo_operacion == OK) {

	// 	t_buffer* buffer = (t_buffer*)recibir_buffer(socket);
	// 	t_list* lista = deserializar_lista_strings(buffer);

	// 	loggear_lista_strings(lista);

  //       tripulante->id_tripulante = atoi(list_get(lista,0));
  //       char** posicion = string_split(list_get(lista,1),"|");
  //       tripulante->pos_x = atoi(posicion[0]);
  //       tripulante->pos_y = atoi(posicion[1]);
	// 	tripulante->tarea_actual = list_get(lista,2);

  //       miLogInfo("Se pasa el tripulante a la cola de NEW");

  //       sem_wait(&mutexNEW);
  //       list_add(new_list, tripulante);
  //       sem_post(&mutexNEW);

	// 	miLogInfo("Tripulante iniciado correctamente");

  //       //2. Una vez realizadas las tareas de preparacion, aviso a Discordiador que estoy listo
  //       tripulante_listo(tripulante);
  // } else if (codigo_operacion == FAIL){
  //       miLogInfo("ERROR: TRIPULANTE NO INICIADO \n");
	// }

  //printf("De responderme q no nos conoce, decodifico nuestas tareas y se las mando\n");
  //printf("Si nos conoce, me guardo la tarea a realizr para cuando el discordiador me mande a EXEC\n"); 
  //and others... but you get it
  //un mutex hasta q estoy en exec?

return;
}

void* planificar_tripulante(void* trip_dato){
  Tripulante* trip = (Tripulante*) trip_dato;
  if((strncmp(configuracion->algoritmo,"FIFO",4))==0){
    planificar_con_FIFO(trip);
  }
  else if((strncmp(configuracion->algoritmo,"RR",2))==0){
    //planificar_con_RR(trip);
  }
  else{
    miLogError("\nNo se pudo detectar algoritmo");
  }

  return NULL;
}

void planificar_con_FIFO(Tripulante* trip){
  sem_wait(&semaforoEXEC);
  sem_wait(&mutexREADY);
  list_add(execute_list,list_remove(ready_list,0));
  sem_post(&mutexREADY);
  
  miLogInfo("\nSe pasa tripulante a estado EXEC\n");
  //ejecutar_proxima_tarea(trip);
}

void execute(Tripulante tripulante){

}