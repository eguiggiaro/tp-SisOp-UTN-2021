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
        obtener_tarea(list_get(lista,2),tripulante->tarea_actual);
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

  //3. Una vez que el tripulante esta listo, se llama a comenzar_ejecucion(tripulante)
  comenzar_ejecucion(tripulante);

return;
}

void ejecutar_proxima_tarea(Tripulante* tripulante){
  if(strncmp(configuracion->algoritmo,"FIFO",4)==0){
    ejecutar_proxima_tarea_FIFO(tripulante);
  }
  else if(strncmp(configuracion->algoritmo,"RR",2)==0){
    //ejecutar_proxima_tarea_RR(tripulante);
  }
  else{
    miLogError("\nAlgoritmo no seteado!");
  }
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
  trip->tarea_actual = NULL;
}

void generar_oxigeno_FIFO(Tripulante* trip){
  //Completar pasos 1, 2, 3 y 4.
  miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO");
  trip->tarea_actual = NULL;
}

void consumir_oxigeno_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_OXIGENO");
  trip->tarea_actual = NULL;
}

void consumir_comida_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_COMIDA");
  trip->tarea_actual = NULL;
}

void generar_basura_FIFO(Tripulante* trip){
  //Completar pasos 1, 2, 3 y 4.
  miLogInfo("\nComienza ejecucion de GENERAR_BASURA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_BASURA");
  trip->tarea_actual = NULL;
}

void descartar_basura_FIFO(Tripulante* trip){
  //Completar pasos 1, 2 y 3.
  miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de DESCARTAR_BASURA");
  trip->tarea_actual = NULL;
}

void tarea_generica_FIFO(Tripulante* trip){
  //Moverse a ubicacion.
  miLogInfo("\nComienza ejecucion de TAREA GENERICA");
  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de TAREA GENERICA");
  trip->tarea_actual = NULL;
}

void bloquear_tripulante(Tripulante* trip){

  int indice;
  Tripulante* trip_auxiliar;

  for(int i =0; i<list_size(execute_list);i++){

    trip_auxiliar = list_get(execute_list,i);

    if(trip->id_tripulante == trip_auxiliar->id_tripulante){
      indice = i;
    }
  }

  if(indice!=NULL){
  //Se saca tripulante de cola de EXEC y se pasa a cola de BLOCK.
	sem_wait(&mutexBLOCK);
  sem_wait(&mutexEXEC); //esta bien?
  //comienza seccion critica
  //list_remove() devuelve el tripulante que se elimina de la lista
  trip_auxiliar = list_remove(execute_list,indice);
  list_add(exit_list,trip_auxiliar);
  //finaliza seccion critica
	sem_post(&mutexBLOCK);
  sem_post(&mutexEXEC);

  //libero un lugar en la cola de EXEC
  sem_post(&semaforoEXEC);
	trip->estado = bloqueado_io;
	miLogInfo("\nSe pasa el tripulante a la cola de BLOCK\n");
  }

}

void comenzar_ejecucion(Tripulante* tripulante){

  while(1){

    if(tripulante->tripulante_despierto){

      if(tripulante->tarea_actual == NULL){
        //pedir_proxima_tarea(tripulante); la tarea actual se borra luego de ejecutada
      }

      if(tripulante->completo_tareas){ //atributo booleano que se setea en TRUE cuando miram informa que no tiene mas para ejecutar
        finalizar_tripulante(tripulante);
        break;
      }

      ejecutar_proxima_tarea(tripulante);
    }
    else{
      sem_wait(&tripulante->semaforo_trip);
    }
  }

}
