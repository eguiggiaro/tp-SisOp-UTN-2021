#include "tripulante.h"

void *inicializar_tripulante(Tripulante* tripulante){
  //Para pruebas, en caso de que falle la comunicacion con MiRAM:
  // aux_id_tripulante++;

  // tripulante->id_tripulante = aux_id_tripulante;
  // tripulante->pos_x = 1;
  // tripulante->pos_y = 1;

  //1. Aviso a MIRAM que deseo iniciar, indicando a que patota pertenezco.
   t_paquete* paquete = crear_paquete(INICIAR_TRIPULANTE);
   t_buffer* buffer;
   
   char* id_patota = string_itoa(tripulante->id_patota);
   t_list* lista_mensajes = list_create();
   list_add(lista_mensajes,id_patota);
   buffer = serializar_lista_strings(lista_mensajes);
   paquete ->buffer = buffer;
  
   enviar_paquete(paquete, socket_miram);

   //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(socket_miram);
	if (codigo_operacion == OK) {

		t_buffer* buffer = (t_buffer*)recibir_buffer(socket_miram);
		t_list* lista = deserializar_lista_strings(buffer);

		loggear_lista_strings(lista);

        tripulante->id_tripulante = atoi(list_get(lista,0));
        char** posicion = string_split(list_get(lista,1),"|");
        tripulante->pos_x = atoi(posicion[0]);
        tripulante->pos_y = atoi(posicion[1]);
	    	tripulante->tarea_actual = list_get(lista,2);
        sem_init(&(tripulante->semaforo_trip),0,0);
        //sem_destroy...

        miLogInfo("Se pasa el tripulante a la cola de NEW");

        sem_wait(&mutexNEW);
        list_add(new_list, tripulante);
        sem_post(&mutexNEW);

		miLogInfo("Tripulante iniciado correctamente");

        //2. Una vez realizadas las tareas de preparacion, aviso a Discordiador que estoy listo
        tripulante_listo(tripulante);

        list_destroy(lista);
        eliminar_buffer(buffer);
  } else if (codigo_operacion == FAIL){
        miLogInfo("ERROR: TRIPULANTE NO INICIADO \n");
	}

  //printf("De responderme q no nos conoce, decodifico nuestas tareas y se las mando\n");
  //printf("Si nos conoce, me guardo la tarea a realizr para cuando el discordiador me mande a EXEC\n"); 
  //and others... but you get it
  //un mutex hasta q estoy en exec?

return;
}

void* planificar_tripulante(Tripulante* trip){
  while(planificacion_activada==true){
    
  sem_wait(&semaforoEXEC);
  sem_wait(&mutexREADY);
  //tomo proximo tripulante de la cola de READY y lo paso a EXEC
  list_add(execute_list,list_remove(ready_list,0));
  sem_post(&mutexREADY);
  trip->estado = trabajando;
  miLogInfo("\nSe pasa tripulante a estado EXEC\n");

  //le doy signal al semaforo del tripulante
  sem_post(&(trip->semaforo_trip));

  printf("\nProxima tarea a ejecutar: %s", (trip->tarea_actual)->nombre_tarea);
  ejecutar_proxima_tarea_FIFO(trip);
  }

  return NULL;
}

void ejecutar_proxima_tarea_FIFO(Tripulante* trip){

  char* tarea = (trip->tarea_actual)->nombre_tarea;

  if(strncmp(tarea,"GENERAR_COMIDA", string_length(tarea))==0){
    generar_comida_FIFO(trip);
  }
  else if(strncmp(tarea,"GENERAR_OXIGENO", string_length(tarea))==0){
    generar_oxigeno_FIFO(trip);
  }
  else if(strncmp(tarea,"CONSUMIR_OXIGENO", string_length(tarea))==0){
    consumir_oxigeno_FIFO(trip);
  }
  else if(strncmp(tarea," CONSUMIR_COMIDA", string_length(tarea))==0){
    consumir_comida_FIFO(trip);
  }
  else if(strncmp(tarea,"GENERAR_BASURA", string_length(tarea))==0){
    generar_basura_FIFO(trip);
  }
  else if(strncmp(tarea,"DESCARTAR_BASURA", string_length(tarea))==0){
    descartar_basura_FIFO(trip);
  }
  else
  {
    tarea_generica_FIFO(trip);
  }
  
}

void generar_comida_FIFO(Tripulante* trip){
  //Completar pasos 1, 2, 3 y 4.
  miLogInfo("\nComienza ejecucion de GENERAR_COMIDA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_COMIDA");
}

void generar_oxigeno_FIFO(Tripulante* trip){
  //Completar pasos 1, 2, 3 y 4.
  miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO");
}

void consumir_oxigeno_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_OXIGENO");
}

void consumir_comida_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_COMIDA");
}

void generar_basura_FIFO(Tripulante* trip){
  //Completar pasos 1, 2, 3 y 4.
  miLogInfo("\nComienza ejecucion de GENERAR_BASURA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_BASURA");
}

void descartar_basura_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de DESCARTAR_BASURA");
}

void tarea_generica_FIFO(Tripulante* trip){
  //Moverse a ubicacion.
  miLogInfo("\nComienza ejecucion de TAREA GENERICA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de TAREA GENERICA");
}

void bloquear_tripulante(Tripulante* trip){

  int indice;

  for(int i =0; i<list_size(execute_list);i++){

    Tripulante* trip_auxiliar = list_get(execute_list,i);

    if(trip->id_tripulante == trip_auxiliar->id_tripulante){
      indice = i;
    }
  }

  if(indice!=NULL){
  //Se saca tripulante de cola de EXEC y se pasa a cola de BLOCK.
	sem_wait(&mutexBLOCK);
    list_add(blocked_io, list_remove(execute_list,indice));
	sem_post(&mutexBLOCK);
  //libero un lugar en la cola de EXEC
  sem_post(&semaforoEXEC);
	trip->estado = bloqueado_io;
	miLogInfo("\nSe pasa el tripulante a la cola de BLOCK\n");
  }
}

