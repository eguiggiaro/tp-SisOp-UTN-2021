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
        //aviso cambio de cola/estado a MIRAM
        //informar_cambio_de_cola_miram(string_itoa(tripulante->id_tripulante),"NEW");

		miLogInfo("Tripulante iniciado correctamente");

        //2. Una vez realizadas las tareas de preparacion, aviso a Discordiador que estoy listo
        tripulante_listo(tripulante);

        list_destroy(lista);
        eliminar_buffer(buffer);
  } else if (codigo_operacion == FAIL){
        miLogInfo("ERROR: TRIPULANTE NO INICIADO \n");
	}

  sem_post(&semaforoREADY);

  //3. Una vez que el tripulante esta listo, se llama a comenzar_ejecucion(tripulante)
  comenzar_ejecucion(tripulante);

return;
}


void pedir_proxima_tarea(Tripulante* un_tripulante){
  miLogInfo("\nSe pide a MIRAM la proxima tarea del tripulante %d",un_tripulante->id_tripulante);
  //1. Aviso a MIRAM que deseo iniciar, indicando a que patota pertenezco.
   t_paquete* paquete = crear_paquete(TAREA_SIGUIENTE);
   t_buffer* buffer;
   
   t_list* lista_mensajes = list_create();
   list_add(lista_mensajes,string_itoa(un_tripulante->id_tripulante));
   buffer = serializar_lista_strings(lista_mensajes);
   paquete ->buffer = buffer;
   enviar_paquete(paquete, socket_miram);

   //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(socket_miram);
	if (codigo_operacion == OK) {

		t_buffer* buffer = (t_buffer*)recibir_buffer(socket_miram);
		t_list* lista = deserializar_lista_strings(buffer);
    char* tareas = list_get(lista,0);

    if (tareas[0] == '$')
    {
        un_tripulante->completo_tareas=true;

    } else {

        Tarea* una_tarea = malloc(sizeof(Tarea));
        obtener_tarea(list_get(lista,0),una_tarea);
        un_tripulante->tarea_actual = una_tarea;
    }
        //sem_destroy...

        list_destroy(lista);
        eliminar_buffer(buffer);
  } else if (codigo_operacion == FAIL){
        miLogInfo("ERROR: TAREA NO EXISTE \n");
	}

}


void ejecutar_proxima_tarea(Tripulante* tripulante){
  if(strncmp(configuracion->algoritmo,"FIFO",4)==0){
    ejecutar_proxima_tarea_FIFO(tripulante);
  }
  else if(strncmp(configuracion->algoritmo,"RR",2)==0){
    ejecutar_proxima_tarea_RR(tripulante);
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
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Comida.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;

}

void generar_oxigeno_FIFO(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Oxigeno.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;

}

void consumir_oxigeno_FIFO(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Oxigeno.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void consumir_comida_FIFO(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

 miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Comida.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void generar_basura_FIFO(Tripulante* trip){
  //Le informo la tarea a STORE para verificar la existencia del archivo Basura.ims
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Basura.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void descartar_basura_FIFO(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if(!trip->recibio_input_store){
    if(tarea_informada(trip->id_tripulante,tarea,parametro)){
      miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
    }
    else{
      miLogError("\nError verificando la existencia de Basura.ims");
    }
    trip->recibio_input_store = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void tarea_generica_FIFO(Tripulante* trip){
  //Moverse a ubicacion.
  char* nombre_tarea = (trip->tarea_actual)->nombre_tarea;
  miLogInfo("\nComienza ejecucion de: %s para el tripulante: %d",nombre_tarea,trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),nombre_tarea);

  if(!trip->realizo_movimientos_tarea){
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
  miLogInfo("\nFinaliza ejecucion de: %s para el tripulante: %d",nombre_tarea,trip->id_tripulante);
  avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),nombre_tarea);
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void ejecutar_proxima_tarea_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;

  if(strncmp(tarea,"GENERAR_COMIDA", string_length(tarea))==0){
    generar_comida_RR(trip);
  }
  else if(strncmp(tarea,"GENERAR_OXIGENO", string_length(tarea))==0){
    generar_oxigeno_RR(trip);
  }
  else if(strncmp(tarea,"CONSUMIR_OXIGENO", string_length(tarea))==0){
    consumir_oxigeno_RR(trip);
  }
  else if(strncmp(tarea," CONSUMIR_COMIDA", string_length(tarea))==0){
    consumir_comida_RR(trip);
  }
  else if(strncmp(tarea,"GENERAR_BASURA", string_length(tarea))==0){
    generar_basura_RR(trip);
  }
  else if(strncmp(tarea,"DESCARTAR_BASURA", string_length(tarea))==0){
    descartar_basura_RR(trip);
  }
  else
  {
    tarea_generica_RR(trip);
  }
}

void generar_comida_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Comida.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void generar_oxigeno_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Oxigeno.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void consumir_oxigeno_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Oxigeno.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void consumir_comida_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Comida.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void generar_basura_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Basura.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void descartar_basura_RR(Tripulante* trip){
  char* tarea = (trip->tarea_actual)->nombre_tarea;
  char* parametro = (trip->tarea_actual)->parametros;

  miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
    if(!trip->recibio_input_store){
      if(tarea_informada(trip->id_tripulante,tarea,parametro)){
        miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
      }
      else{
        miLogError("\nError verificando la existencia de Basura.ims");
      }
      trip->recibio_input_store = true;
    }

    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");

    if(trip->quantum >0){
      for(int i = 0; i<trip->quantum;i++){
        trip->quantum--;
      }
    }
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }

}

void tarea_generica_RR(Tripulante* trip){
  //Moverse a ubicacion.
  char* nombre_tarea = (trip->tarea_actual)->nombre_tarea;
  miLogInfo("\nComienza ejecucion de: %s para el tripulante: %d",nombre_tarea,trip->id_tripulante);
  avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),nombre_tarea);

  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if(trip->quantum > 0){

    if(!trip->realizo_movimientos_tarea){
      mover_tripulante_RR(trip);
      trip->realizo_movimientos_tarea = true;
    }

    if(trip->quantum > 0){
      int retardo = configuracion->retardo_ciclo_cpu;
      int ciclos_cpu = sleep(retardo*((trip->tarea_actual)->tiempo));
      miLogInfo("\nFinaliza ejecucion de: %s para el tripulante: %d",nombre_tarea,trip->id_tripulante);
      avisar_fin_tarea_bitacora(string_itoa(trip->id_tripulante),nombre_tarea);

      if(trip->quantum >0){
        for(int i = 0; i<trip->quantum;i++){
          trip->quantum--;
        }
      }
      //seteo parametros para proxima tarea
      trip->tarea_actual = NULL;
      trip->realizo_movimientos_tarea = false;
      trip->recibio_input_store = false;
    }

  }else{
    //pasar tripulante de EXEC a READY (caso en que el tripulante no tiene quantum disponible)
  }
  
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
  list_add(blocked_io,trip_auxiliar);
  //finaliza seccion critica
	sem_post(&mutexBLOCK);
  sem_post(&mutexEXEC);
  //aviso cambio de estado/cola a MIRAM
  //informar_cambio_de_cola_miram(string_itoa(trip_auxiliar->id_tripulante),"BLOCK");

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
        pedir_proxima_tarea(tripulante); //la tarea actual se borra luego de ejecutada
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

void mover_tripulante(Tripulante* trip){
    //Me muevo desde la posicion actual hasta donde pide la tarea.
    int distancia_x;
    int distancia_y;
    int x_origen = trip->pos_x;
    int y_origen = trip->pos_y;
    int x_destino = atoi((trip->tarea_actual)->pos_x);
    int y_destino = atoi((trip->tarea_actual)->pos_y);

    //parametro para enviar a bitacora
    char* id_trip = string_itoa(trip->id_tripulante);


    //Primero me muevo linealmente en el eje X
    if(x_origen > x_destino){
      distancia_x = x_origen - x_destino;
      for(int i = 0; i<distancia_x; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* x_destino_b = string_itoa((trip->pos_x)-1);
        char* y_destino_b = y_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_x)--;
        //aviso a miram
        avisar_movimiento_miram(trip,"X");
      }
    }
    else if(x_origen < x_destino){
      distancia_x = x_destino - x_origen;
      for(int i = 0; i<distancia_x; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* x_destino_b = string_itoa((trip->pos_x)+1);
        char* y_destino_b = y_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_x)++;
        //aviso a miram
        avisar_movimiento_miram(trip,"X");
      }
    }
    else{
      distancia_x = 0;
    }

    //Despues hago lo mismo en el eje Y
    if(y_origen > y_destino){
      distancia_y = y_origen - y_destino;
      for(int i = 0; i<distancia_y; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* y_destino_b = string_itoa((trip->pos_y)-1);
        char* x_destino_b = x_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_y)--;
        //aviso a miram
        avisar_movimiento_miram(trip,"Y");
      }
    }
    else if(y_origen < y_destino){
      distancia_y = y_destino - y_origen;
      for(int i = 0; i<distancia_y; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* y_destino_b = string_itoa((trip->pos_y)+1);
        char* x_destino_b = x_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_y)++;
        //aviso a miram
        avisar_movimiento_miram(trip,"Y");
      }
    }
    else{
      distancia_y = 0;
    }
}

void* mover_tripulante_RR(Tripulante* trip){
  //Me muevo desde la posicion actual hasta donde pide la tarea.
    int distancia_x;
    int distancia_y;
    int x_origen = trip->pos_x;
    int y_origen = trip->pos_y;
    int x_destino = atoi((trip->tarea_actual)->pos_x);
    int y_destino = atoi((trip->tarea_actual)->pos_y);

    //parametro para enviar a bitacora
    char* id_trip = string_itoa(trip->id_tripulante);


    //Primero me muevo linealmente en el eje X
    if(x_origen > x_destino){
      distancia_x = x_origen - x_destino;
      for(int i = 0; i<distancia_x; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* x_destino_b = string_itoa((trip->pos_x)-1);
        char* y_destino_b = y_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_x)--;
        //aviso a miram
        avisar_movimiento_miram(trip,"X");

        //se consume una unidad de quantum
        trip->quantum--;

        if(trip->quantum <= 0){
          return NULL; //si no me queda quantum salgo del loop y termino funcion
        }

      }
    }
    else if(x_origen < x_destino){
      distancia_x = x_destino - x_origen;
      for(int i = 0; i<distancia_x; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* x_destino_b = string_itoa((trip->pos_x)+1);
        char* y_destino_b = y_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_x)++;
        //aviso a miram
        avisar_movimiento_miram(trip,"X");

        //se consume una unidad de quantum
        trip->quantum--;

        if(trip->quantum <= 0){
          return NULL; //si no me queda quantum salgo del loop y termino funcion
        }
      }
    }
    else{
      distancia_x = 0;
    }

    //Despues hago lo mismo en el eje Y
    if(y_origen > y_destino){
      distancia_y = y_origen - y_destino;
      for(int i = 0; i<distancia_y; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* y_destino_b = string_itoa((trip->pos_y)-1);
        char* x_destino_b = x_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_y)--;
        //aviso a miram
        avisar_movimiento_miram(trip,"Y");

        //se consume una unidad de quantum
        trip->quantum--;

        if(trip->quantum <= 0){
          return NULL; //si no me queda quantum salgo del loop y termino funcion
        }
      }
    }
    else if(y_origen < y_destino){
      distancia_y = y_destino - y_origen;
      for(int i = 0; i<distancia_y; i++){
        //resto de parametros para enviar a bitacora
        char* x_origen_b = string_itoa(trip->pos_x);
        char* y_origen_b = string_itoa(trip->pos_y);
        char* y_destino_b = string_itoa((trip->pos_y)+1);
        char* x_destino_b = x_origen_b;
        char* origen = string_new();
        string_append(&origen,x_origen_b);
        string_append(&origen, "|");
        string_append(&origen, y_origen_b);
        char* destino = string_new();
        string_append(&destino,x_destino_b);
        string_append(&destino,"|");
        string_append(&destino, y_destino_b);

        avisar_movimiento_bitacora(id_trip,origen,destino);
        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        //realizo movimiento
        (trip->pos_y)++;
        //aviso a miram
        avisar_movimiento_miram(trip,"Y");

        //se consume una unidad de quantum
        trip->quantum--;

        if(trip->quantum <= 0){
          return NULL; //si no me queda quantum salgo del loop y termino funcion
        }
      }
    }
    else{
      distancia_y = 0;
    }
}