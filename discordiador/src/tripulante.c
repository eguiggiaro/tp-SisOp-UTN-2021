#include "tripulante.h"

void *inicializar_tripulante(Tripulante *tripulante)
{
  //Para pruebas, en caso de que falle la comunicacion con MiRAM:
  // aux_id_tripulante++;

  // tripulante->id_tripulante = aux_id_tripulante;
  // tripulante->pos_x = 1;
  // tripulante->pos_y = 1;

  //1. Aviso a MIRAM que deseo iniciar, indicando a que patota pertenezco.
  t_paquete *paquete = crear_paquete(INICIAR_TRIPULANTE);
  t_buffer *buffer;

  tripulante->socket_miram = iniciar_conexion_miram(configuracion->ip_mi_ram_hq, configuracion->puerto_mi_ram_hq);
  tripulante->socket_store = iniciar_conexion_store(configuracion->ip_i_mongo_store, configuracion->puerto_i_mongo_store);

  char *id_patota = string_itoa(tripulante->id_patota);
  t_list *lista_mensajes = list_create();
  list_add(lista_mensajes, id_patota);
  buffer = serializar_lista_strings(lista_mensajes);
  paquete->buffer = buffer;

  enviar_paquete(paquete, tripulante->socket_miram);

  //recibe respuesta de destino
  op_code codigo_operacion = recibir_operacion(tripulante->socket_miram);
  if (codigo_operacion == OK)
  {

    t_buffer *buffer = (t_buffer *)recibir_buffer(tripulante->socket_miram);
    t_list *lista = deserializar_lista_strings(buffer);

    loggear_lista_strings(lista);

    tripulante->id_tripulante = atoi(list_get(lista, 0));
    char **posicion = string_split(list_get(lista, 1), "|");
    tripulante->pos_x = atoi(posicion[0]);
    tripulante->pos_y = atoi(posicion[1]);
    tripulante->aviso_inicio_tarea_store = false;
    obtener_tarea(list_get(lista, 2), tripulante->tarea_actual);

    list_add(tripulantes_totales, tripulante);
    //sem_destroy...

    miLogInfo("Se pasa el tripulante a la cola de NEW");

    pthread_mutex_lock(&mutexNEW);
    list_add(new_list, tripulante);
    pthread_mutex_unlock(&mutexNEW);
    //aviso cambio de cola/estado a MIRAM
    //informar_cambio_de_cola_miram(string_itoa(tripulante->id_tripulante),"NEW");

    miLogInfo("Tripulante iniciado correctamente");

    //2. Una vez realizadas las tareas de preparacion, aviso a Discordiador que estoy listo
    tripulante_listo(tripulante);

    list_destroy(lista);
    eliminar_buffer(buffer);
  }
  else if (codigo_operacion == FAIL)
  {
    miLogInfo("ERROR: TRIPULANTE NO INICIADO \n");
  }

  sem_post(&semaforoREADY);

  pthread_mutex_init(&tripulante->semaforo_trip, NULL);

  miLogDebug("Tripulante %d: por comenzar ejecucion", tripulante->id_tripulante);

  //3. Una vez que el tripulante esta listo, se llama a comenzar_ejecucion(tripulante)
  comenzar_ejecucion(tripulante);
  return;
}

void pedir_proxima_tarea(Tripulante *un_tripulante)
{
  miLogInfo("\nSe pide a MIRAM la proxima tarea del tripulante %d", un_tripulante->id_tripulante);
  //1. Aviso a MIRAM que deseo iniciar, indicando a que patota pertenezco.
  t_paquete *paquete = crear_paquete(TAREA_SIGUIENTE);
  t_buffer *buffer;

  t_list *lista_mensajes = list_create();
  list_add(lista_mensajes, string_itoa(un_tripulante->id_tripulante));
  buffer = serializar_lista_strings(lista_mensajes);
  paquete->buffer = buffer;
  enviar_paquete(paquete, un_tripulante->socket_miram);

  //recibe respuesta de destino
  op_code codigo_operacion = recibir_operacion(un_tripulante->socket_miram);
  if (codigo_operacion == OK)
  {

    t_buffer *buffer = (t_buffer *)recibir_buffer(un_tripulante->socket_miram);
    t_list *lista = deserializar_lista_strings(buffer);
    char *tareas = list_get(lista, 0);

    if (tareas[0] == '$')
    {
      un_tripulante->completo_tareas = true;
      miLogInfo("\nEl tripulante: %d finalizo todas sus tareas.", un_tripulante->id_tripulante);
    }
    else
    {

      Tarea *una_tarea = malloc(sizeof(Tarea));
      obtener_tarea(list_get(lista, 0), una_tarea);
      un_tripulante->tarea_actual = una_tarea;
    }
    //sem_destroy...

    list_destroy(lista);
    eliminar_buffer(buffer);
  }
  else
  {
    miLogInfo("ERROR: TAREA NO EXISTE \n");
  }
}

void ejecutar_proxima_tarea(Tripulante *tripulante)
{
  if (strncmp(configuracion->algoritmo, "FIFO", 4) == 0)
  {
    ejecutar_proxima_tarea_FIFO(tripulante);
  }
  else if (strncmp(configuracion->algoritmo, "RR", 2) == 0)
  {
    ejecutar_proxima_tarea_RR(tripulante);
  }
  else
  {
    miLogError("\nAlgoritmo no seteado!");
  }
}

void ejecutar_proxima_tarea_FIFO(Tripulante *trip)
{

  char *tarea = (trip->tarea_actual)->nombre_tarea;

  if (strncmp(tarea, "GENERAR_COMIDA", string_length(tarea)) == 0)
  {
    generar_comida_FIFO(trip);
  }
  else if (strncmp(tarea, "GENERAR_OXIGENO", string_length(tarea)) == 0)
  {
    generar_oxigeno_FIFO(trip);
  }
  else if (strncmp(tarea, "CONSUMIR_OXIGENO", string_length(tarea)) == 0)
  {
    consumir_oxigeno_FIFO(trip);
  }
  else if (strncmp(tarea, "CONSUMIR_COMIDA", string_length(tarea)) == 0)
  {
    consumir_comida_FIFO(trip);
  }
  else if (strncmp(tarea, "GENERAR_BASURA", string_length(tarea)) == 0)
  {
    generar_basura_FIFO(trip);
  }
  else if (strncmp(tarea, "DESCARTAR_BASURA", string_length(tarea)) == 0)
  {
    descartar_basura_FIFO(trip);
  }
  else
  {
    tarea_generica_FIFO(trip);
  }
}

void generar_comida_FIFO(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;
  miLogInfo("\nComienza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Comida.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "GENERAR_COMIDA");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void generar_oxigeno_FIFO(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;

  miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Oxigeno.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void consumir_oxigeno_FIFO(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;

  miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Oxigeno.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "CONSUMIR_OXIGENO");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void consumir_comida_FIFO(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;

  miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Comida.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "CONSUMIR_COMIDA");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void generar_basura_FIFO(Tripulante *trip)
{
  //Le informo la tarea a STORE para verificar la existencia del archivo Basura.ims
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;

  miLogInfo("\nComienza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Basura.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "GENERAR_BASURA");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void descartar_basura_FIFO(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;
  char *parametro = (trip->tarea_actual)->parametros;
  int retardo = configuracion->retardo_ciclo_cpu;

  miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
  //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
  }

  if (!trip->recibio_input_store)
  {
    //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
    bloquear_tripulante_io(trip);
    if (tarea_informada(trip, tarea, parametro))
    {
      miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
    }
    else
    {
      miLogError("\nError verificando la existencia de Basura.ims");
    }
    //Consume un ciclo de CPU
    (trip->tarea_actual)->tiempo--;
    int ciclos_cpu = sleep(retardo * (1));
    miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
    //Se desbloquea
    desbloquear_tripulante_io(trip);

    trip->recibio_input_store = true;
  }

  pthread_mutex_lock(&trip->semaforo_trip);

  if (trip->estado == trabajando)
  {
    //Consume ciclos de CPU restantes
    int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
    miLogInfo("\nFinaliza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
    avisar_fin_tarea_bitacora(trip, "DESCARTAR_BASURA");
    //seteo parametros para proxima tarea
    trip->tarea_actual = NULL;
    trip->realizo_movimientos_tarea = false;
    trip->recibio_input_store = false;
  }
}

void tarea_generica_FIFO(Tripulante *trip)
{
  //Moverse a ubicacion.
  char *nombre_tarea = (trip->tarea_actual)->nombre_tarea;

  if (!trip->realizo_movimientos_tarea)
  {
    mover_tripulante(trip);
    trip->realizo_movimientos_tarea = true;
    miLogInfo("\nComienza ejecucion de: %s para el tripulante: %d", nombre_tarea, trip->id_tripulante);
    avisar_inicio_tarea_bitacora(trip, nombre_tarea);
  }

  int retardo = configuracion->retardo_ciclo_cpu;
  int ciclos_cpu = sleep(retardo * ((trip->tarea_actual)->tiempo));
  avisar_fin_tarea_bitacora(trip, nombre_tarea);
  miLogInfo("\nFinaliza ejecucion de: %s para el tripulante: %d", nombre_tarea, trip->id_tripulante);
  //seteo parametros para proxima tarea
  trip->tarea_actual = NULL;
  trip->realizo_movimientos_tarea = false;
  trip->recibio_input_store = false;
}

void ejecutar_proxima_tarea_RR(Tripulante *trip)
{
  char *tarea = (trip->tarea_actual)->nombre_tarea;

  if (strncmp(tarea, "GENERAR_COMIDA", string_length(tarea)) == 0)
  {
    generar_comida_RR(trip);
  }
  else if (strncmp(tarea, "GENERAR_OXIGENO", string_length(tarea)) == 0)
  {
    generar_oxigeno_RR(trip);
  }
  else if (strncmp(tarea, "CONSUMIR_OXIGENO", string_length(tarea)) == 0)
  {
    consumir_oxigeno_RR(trip);
  }
  else if (strncmp(tarea, "CONSUMIR_COMIDA", string_length(tarea)) == 0)
  {
    consumir_comida_RR(trip);
  }
  else if (strncmp(tarea, "GENERAR_BASURA", string_length(tarea)) == 0)
  {
    generar_basura_RR(trip);
  }
  else if (strncmp(tarea, "DESCARTAR_BASURA", string_length(tarea)) == 0)
  {
    descartar_basura_RR(trip);
  }
  else
  {
    tarea_generica_RR(trip);
  }
}

void generar_comida_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de GENERAR_COMIDA para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_COMIDA");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Comida.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }

      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void generar_oxigeno_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_OXIGENO");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Oxigeno.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }


      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void consumir_oxigeno_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de CONSUMIR_OXIGENO para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_OXIGENO");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Oxigeno.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Oxigeno.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }

      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void consumir_comida_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de CONSUMIR_COMIDA para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"CONSUMIR_COMIDA");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Comida.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Comida.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }

      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void generar_basura_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de GENERAR_BASURA para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"GENERAR_BASURA");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Basura.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }

      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void descartar_basura_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    char *tarea = (trip->tarea_actual)->nombre_tarea;
    char *parametro = (trip->tarea_actual)->parametros;
    int retardo = configuracion->retardo_ciclo_cpu;

    miLogInfo("\nComienza ejecucion de DESCARTAR_BASURA para el tripulante: %d", trip->id_tripulante);
    //avisar_inicio_tarea_bitacora(string_itoa(trip->id_tripulante),"DESCARTAR_BASURA");
    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->recibio_input_store)
      {
        //El tripulante pasa a la cola de BLOCKED_IO mientras espera la respuesta de Store.
        bloquear_tripulante_io(trip);
        if (tarea_informada(trip, tarea, parametro))
        {
          miLogInfo("\nSe verifico la existencia de Basura.ims correctamente");
        }
        else
        {
          miLogError("\nError verificando la existencia de Basura.ims");
        }
        //Consume un ciclo de CPU
        (trip->tarea_actual)->tiempo--;
        int ciclos_cpu = sleep(retardo * (1));
        miLogInfo("El tripulante: %d consumio un ciclo de CPU por peticion I/O\n", trip->id_tripulante);
        //Se desbloquea
        desbloquear_tripulante_io(trip);

        trip->recibio_input_store = true;

        pthread_mutex_lock(&trip->semaforo_trip);
      }

      if (trip->estado == trabajando)
      {

        //Consume ciclos de CPU restantes
        for(int i = 0; i<trip->tarea_actual->tiempo; i++)
        {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

        }
        miLogInfo("\nFinaliza ejecucion de GENERAR_OXIGENO para el tripulante: %d", trip->id_tripulante);
        avisar_fin_tarea_bitacora(trip, "GENERAR_OXIGENO");

        //seteo parametros para proxima tarea
        trip->tarea_actual = NULL;
        trip->realizo_movimientos_tarea = false;
        trip->recibio_input_store = false;

      }
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void tarea_generica_RR(Tripulante *trip)
{
  //Antes de empezar a ejecutar verifico que el tripulante posea quantum
  if (trip->quantum > 0)
  {
    //Moverse a ubicacion.
    char *nombre_tarea = (trip->tarea_actual)->nombre_tarea;
    miLogInfo("\nComienza ejecucion de: %s para el tripulante: %d", nombre_tarea, trip->id_tripulante);

    if (!trip->realizo_movimientos_tarea)
    {
      mover_tripulante_RR(trip);
    }

    if (trip->quantum > 0)
    {
      if (!trip->aviso_inicio_tarea_store)
      {
        avisar_inicio_tarea_bitacora(trip, nombre_tarea);
        trip->aviso_inicio_tarea_store = true;
      }


      int retardo = configuracion->retardo_ciclo_cpu;

      for(int i = 0; i<trip->tarea_actual->tiempo; i++)
      {
          if (trip->quantum <= 0)
          {
              return;
          }
          int ciclos_cpu = sleep(retardo * (1));
          trip->tarea_actual->tiempo--;
          trip->quantum--;

       }

      miLogInfo("\nFinaliza ejecucion de: %s para el tripulante: %d", nombre_tarea, trip->id_tripulante);
      avisar_fin_tarea_bitacora(trip, nombre_tarea);

      //seteo parametros para proxima tarea
      trip->tarea_actual = NULL;
      trip->realizo_movimientos_tarea = false;
      trip->recibio_input_store = false;
      trip->aviso_inicio_tarea_store = false;
    }
  }
  else
  {
    miLogInfo("\nEl tripulante: %d se quedo sin quantum", trip->id_tripulante);
    pasar_tripulante_de_exec_a_ready(trip);
  }
}

void bloquear_tripulante_io(Tripulante *trip)
{

  int indice;
  Tripulante *trip_auxiliar;
  bool tripulante_encontrado = false;

  for (int i = 0; i < list_size(execute_list); i++)
  {

    trip_auxiliar = list_get(execute_list, i);

    if (trip->id_tripulante == trip_auxiliar->id_tripulante)
    {
      indice = i;
      tripulante_encontrado = true;
    }
  }

  if (tripulante_encontrado)
  {
    //Se saca tripulante de cola de EXEC y se pasa a cola de BLOCK.
    pthread_mutex_lock(&mutexBLOCK);
    pthread_mutex_lock(&mutexEXEC); //esta bien?
    //comienza seccion critica
    //list_remove() devuelve el tripulante que se elimina de la lista
    trip_auxiliar = list_remove(execute_list, indice);
    list_add(blocked_io, trip_auxiliar);
    trip_auxiliar->estado = bloqueado_io;
    //finaliza seccion critica
    pthread_mutex_unlock(&mutexBLOCK);
    pthread_mutex_unlock(&mutexEXEC);
    //aviso cambio de estado/cola a MIRAM
    //informar_cambio_de_cola_miram(string_itoa(trip_auxiliar->id_tripulante),"BLOCK");

    //libero un lugar en la cola de EXEC
    sem_post(&semaforoEXEC);
    trip->estado = bloqueado_io;
    //trip->tripulante_despierto = false;
    miLogInfo("\nSe pasa al tripulante: %d a la cola de BLOCK IO\n", trip->id_tripulante);
    //al liberarse un lugar en la cola de EXEC, replanifico:
    //planificar();
  }
}

void comenzar_ejecucion(Tripulante *tripulante)
{

  pthread_mutex_lock(&tripulante->semaforo_trip);


  while (1)
  {

    if (tripulante->tripulante_despierto)
    {
      if (tripulante->tarea_actual == NULL)
      {
        miLogDebug("Tripulante %d: va a pedir proxima tarea", tripulante->id_tripulante);

        pedir_proxima_tarea(tripulante); //la tarea actual se borra luego de ejecutada
        miLogDebug("Tripulante %d: pidio proxima tarea", tripulante->id_tripulante);
      }

      if (tripulante->completo_tareas)
      { //atributo booleano que se setea en TRUE cuando miram informa que no tiene mas para ejecutar

        miLogDebug("Tripulante %d: por finalizar", tripulante->id_tripulante);
        finalizar_tripulante(tripulante);
        break;
      }
      miLogDebug("Tripulante %d: por ejecutar proxima tarea", tripulante->id_tripulante);
      ejecutar_proxima_tarea(tripulante);
      miLogDebug("Tripulante %d: ejecuto proxima tarea", tripulante->id_tripulante);
    }
    else
    {
      miLogDebug("Tripulante %d: No esta despierto lock del mutex", tripulante->id_tripulante);
      pthread_mutex_lock(&tripulante->semaforo_trip);
    }
  }
}

void mover_tripulante(Tripulante *trip)
{
  //Me muevo desde la posicion actual hasta donde pide la tarea.
  int distancia_x;
  int distancia_y;
  int x_origen = trip->pos_x;
  int y_origen = trip->pos_y;
  int x_destino = atoi((trip->tarea_actual)->pos_x);
  int y_destino = atoi((trip->tarea_actual)->pos_y);

  //parametro para enviar a bitacora
  char *id_trip = string_itoa(trip->id_tripulante);

  //Primero me muevo linealmente en el eje X
  if (x_origen > x_destino)
  {
    distancia_x = x_origen - x_destino;
    for (int i = 0; i < distancia_x; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *x_destino_b = string_itoa((trip->pos_x) - 1);
      char *y_destino_b = y_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_x)--;
      //aviso a miram
      avisar_movimiento_miram(trip, "X");
    }
  }
  else if (x_origen < x_destino)
  {
    distancia_x = x_destino - x_origen;
    for (int i = 0; i < distancia_x; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *x_destino_b = string_itoa((trip->pos_x) + 1);
      char *y_destino_b = y_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_x)++;
      //aviso a miram
      avisar_movimiento_miram(trip, "X");
    }
  }
  else
  {
    distancia_x = 0;
  }

  //Despues hago lo mismo en el eje Y
  if (y_origen > y_destino)
  {
    distancia_y = y_origen - y_destino;
    for (int i = 0; i < distancia_y; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *y_destino_b = string_itoa((trip->pos_y) - 1);
      char *x_destino_b = x_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_y)--;
      //aviso a miram
      avisar_movimiento_miram(trip, "Y");
    }
  }
  else if (y_origen < y_destino)
  {
    distancia_y = y_destino - y_origen;
    for (int i = 0; i < distancia_y; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *y_destino_b = string_itoa((trip->pos_y) + 1);
      char *x_destino_b = x_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_y)++;
      //aviso a miram
      avisar_movimiento_miram(trip, "Y");
    }
  }
  else
  {
    distancia_y = 0;
  }
}

void *mover_tripulante_RR(Tripulante *trip)
{
  //Me muevo desde la posicion actual hasta donde pide la tarea.
  int distancia_x;
  int distancia_y;
  int x_origen = trip->pos_x;
  int y_origen = trip->pos_y;
  int x_destino = atoi((trip->tarea_actual)->pos_x);
  int y_destino = atoi((trip->tarea_actual)->pos_y);

  //parametro para enviar a bitacora
  char *id_trip = string_itoa(trip->id_tripulante);

  //Primero me muevo linealmente en el eje X
  if (x_origen > x_destino)
  {
    distancia_x = x_origen - x_destino;
    for (int i = 0; i < distancia_x; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *x_destino_b = string_itoa((trip->pos_x) - 1);
      char *y_destino_b = y_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_x)--;
      //aviso a miram
      avisar_movimiento_miram(trip, "X");

      //se consume una unidad de quantum
      trip->quantum--;

      if (trip->quantum <= 0)
      {
        return NULL; //si no me queda quantum salgo del loop y termino funcion
      }
    }
  }
  else if (x_origen < x_destino)
  {
    distancia_x = x_destino - x_origen;
    for (int i = 0; i < distancia_x; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *x_destino_b = string_itoa((trip->pos_x) + 1);
      char *y_destino_b = y_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_x)++;
      //aviso a miram
      avisar_movimiento_miram(trip, "X");

      //se consume una unidad de quantum
      trip->quantum--;

      if (trip->quantum <= 0)
      {
        return NULL; //si no me queda quantum salgo del loop y termino funcion
      }
    }
  }
  else
  {
    distancia_x = 0;
  }

  //Despues hago lo mismo en el eje Y
  if (y_origen > y_destino)
  {
    distancia_y = y_origen - y_destino;
    for (int i = 0; i < distancia_y; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *y_destino_b = string_itoa((trip->pos_y) - 1);
      char *x_destino_b = x_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_y)--;
      //aviso a miram
      avisar_movimiento_miram(trip, "Y");

      //se consume una unidad de quantum
      trip->quantum--;

      if (trip->quantum <= 0)
      {
        return NULL; //si no me queda quantum salgo del loop y termino funcion
      }
    }
  }
  else if (y_origen < y_destino)
  {
    distancia_y = y_destino - y_origen;
    for (int i = 0; i < distancia_y; i++)
    {
      //resto de parametros para enviar a bitacora
      char *x_origen_b = string_itoa(trip->pos_x);
      char *y_origen_b = string_itoa(trip->pos_y);
      char *y_destino_b = string_itoa((trip->pos_y) + 1);
      char *x_destino_b = x_origen_b;
      char *origen = string_new();
      string_append(&origen, x_origen_b);
      string_append(&origen, "|");
      string_append(&origen, y_origen_b);
      char *destino = string_new();
      string_append(&destino, x_destino_b);
      string_append(&destino, "|");
      string_append(&destino, y_destino_b);

      miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s", id_trip, origen, destino);
      avisar_movimiento_bitacora(trip, origen, destino);
      //realizo movimiento
      (trip->pos_y)++;
      //aviso a miram
      avisar_movimiento_miram(trip, "Y");

      //se consume una unidad de quantum
      trip->quantum--;

      if (trip->quantum <= 0)
      {
        return NULL; //si no me queda quantum salgo del loop y termino funcion
      }
    }
  }
  else
  {
    distancia_y = 0;
  }

  if (trip->pos_x == x_destino && trip->pos_y == y_destino)
  {
    trip->realizo_movimientos_tarea = true;
  }
}