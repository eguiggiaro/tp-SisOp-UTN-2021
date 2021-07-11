#include<sabotajes_discordiador.h>

void *iniciar_servidor_discordiador(){
    levantar_servidor(atender_request_discordiador, puerto_discordiador);
	return NULL;
}

void atender_request_discordiador(Request *request){
  op_code codigo_operacion = request->codigo_operacion;
	t_buffer *buffer = request->buffer_devolucion;
	int request_fd = request->request_fd;
	t_list *lista;
	t_list *lista_mensajes;

	switch (codigo_operacion)
	{

	case ALERTA_SABOTAJE:
		//recibo los mensajes
		lista_mensajes = list_create();
		//recibo los mensajes
		lista = deserializar_lista_strings(buffer);

        char* posicion = string_new();
        string_append(&posicion, (char*)list_get(lista,0));
        string_append(&posicion, ",");
        string_append(&posicion, (char*)list_get(lista,1));

		miLogInfo("\nMe llego operacion: ALERTA SABOTAJE en el punto: (%s)\n", posicion);

        t_paquete *paquete_devuelto = crear_paquete(OK);
		
        list_add(lista_mensajes, "Operacion recibida correctamente");

		buffer = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer;
		enviar_paquete(paquete_devuelto, request_fd);
		list_destroy(lista_mensajes);
		list_destroy(lista);
		//eliminar_paquete(paquete_devuelto);
		free(request);

        //Creo un thread para atender el sabotaje ya que puede producirse mas de uno simultaneamente
        if (pthread_create(&threadSABOTAJE, NULL, (void*) atender_sabotaje, (char*)posicion) != 0){
		  printf("\nError iniciando thread para atender sabotaje\n");
		}

		pthread_detach(threadSABOTAJE);

		break;

	default:
		miLogInfo("\nMe llego una operacion invalida\n");

		break;
	}
}

double raiz_cuadrada(int numero){

  double root = 1;
  int i = 0;
    
  while (1)
  {
    i = i + 1;
    root = (numero / root + root) / 2;
    if (i == numero + 1) { break; }
  }
    
  return root;
}

void mover_tripulante_a_sabotaje(Tripulante* trip, int x_destino, int y_destino){
    //Me muevo desde la posicion actual hasta donde pide la tarea.
    int distancia_x;
    int distancia_y;
    int x_origen = trip->pos_x;
    int y_origen = trip->pos_y;

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

        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        avisar_movimiento_bitacora(id_trip,origen,destino);
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

        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        avisar_movimiento_bitacora(id_trip,origen,destino);
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

        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        avisar_movimiento_bitacora(id_trip,origen,destino);
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

        miLogInfo("\nSe mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        avisar_movimiento_bitacora(id_trip,origen,destino);
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

void enviar_fcsk(int id_tripulante, int pos_x, int pos_y){

	t_paquete* paquete = crear_paquete(FSCK);
  t_buffer* buffer;

	char* id_auxiliar = string_itoa(id_tripulante);
  char* pos_x_aux = string_itoa(pos_x);
  char* pos_y_aux = string_itoa(pos_y);

	t_list* lista_mensajes = list_create();
	list_add(lista_mensajes,id_auxiliar);
	list_add(lista_mensajes,pos_x_aux);
  list_add(lista_mensajes,pos_y_aux);

	buffer = serializar_lista_strings(lista_mensajes);
  paquete ->buffer = buffer;
  
  enviar_paquete(paquete, socket_store);

  //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(socket_store);
	if (codigo_operacion == OK) {
		miLogInfo("FCSK informado correctamente\n");
	} else if (codigo_operacion == FAIL){
        miLogError("ERROR INFORMANDO FCSK. \n");
	}

	buffer = (t_buffer*)recibir_buffer(socket_store);

	list_destroy(lista_mensajes);

}

void atender_sabotaje(char* posicion){
    char** array_posicion = string_split(posicion, ",");
    int pos_x = atoi(array_posicion[0]);
    int pos_y = atoi(array_posicion[1]);

    //1. Recorro lista general de tripulantes y los duermo a todos
    //TODO: agregar mutex
    for(int i =0; i<list_size(tripulantes_totales);i++){

      Tripulante* tripu = list_get(tripulantes_totales,i);

      miLogInfo("\nSe duerme al tripulante: %d \n",tripu->id_tripulante);

      tripu->tripulante_despierto = false;
      //sem_wait(&(tripu->semaforo_trip)); GENERA DEADLOCK - REVISAR
    }

    //2.1 Recorro lista de EXEC y los paso a BLOCK_IO
    //TODO: ordenar por ID de menor a mayor
    pthread_mutex_lock(&mutexEXEC);
    if(list_size(execute_list) > 0){
    for(int i =0; i<list_size(execute_list);i++){

      Tripulante* tripu = list_remove(execute_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de EXEC a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    pthread_mutex_unlock(&mutexEXEC);

    //2.2 Recorro lista de READY (deberia chequear si no esta vacia?) y los paso a BLOCK_IO
    pthread_mutex_lock(&mutexREADY); 
    if(list_size(ready_list)>0){
    for(int i =0; i<list_size(ready_list);i++){

      Tripulante* tripu = list_remove(ready_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de READY a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    pthread_mutex_unlock(&mutexREADY);

    //2.3 Recorro lista de NEW (deberia chequear si no esta vacia?) y los paso a BLOCK_IO
    pthread_mutex_lock(&mutexNEW);
    if(list_size(new_list)>0){ 
    for(int i =0; i<list_size(new_list);i++){

      Tripulante* tripu = list_remove(new_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de NEW a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    pthread_mutex_unlock(&mutexNEW);

    //3. Calculo al tripulante en la posicion mas cercana y lo paso a cola de BLOCK_EMERGENCIA
    void* tripulante_mas_cercano(Tripulante* un_tripu, Tripulante* otro_tripu){

      double primera_distancia = raiz_cuadrada( (pos_x - un_tripu->pos_x)*(pos_x - un_tripu->pos_x) +
      (pos_y - un_tripu->pos_y)*(pos_y - un_tripu->pos_y) );

      double segunda_distancia = raiz_cuadrada( (pos_x - otro_tripu->pos_x)*(pos_x - otro_tripu->pos_x) +
      (pos_y - otro_tripu->pos_y)*(pos_y - otro_tripu->pos_y) );

      return primera_distancia <= segunda_distancia ? un_tripu : otro_tripu;
    }

    Tripulante* tripulante_elegido = (Tripulante*) list_get_minimum(blocked_io,(void*)tripulante_mas_cercano);

    miLogInfo("\nEl tripulante elegido para llevar a cabo el sabotaje es el: %d \n",tripulante_elegido->id_tripulante);

    //3.1 Movemos al tripulante a la cola de block emergencia
    int indice;
    bool tripulante_encontrado = false;
    for(int i =0; i<list_size(blocked_io);i++){
      Tripulante* trip_aux = list_get(blocked_io,i);
      if(trip_aux->id_tripulante == tripulante_elegido->id_tripulante){
        indice = i;
        tripulante_encontrado = true;
      }
    }

    if(tripulante_encontrado){
      Tripulante* trip_aux = list_remove(blocked_io,indice);
      list_add(blocked_em, trip_aux);
      miLogInfo("\nSe pasa a la cola de BLOCK emergencia al tripulante: %d \n",tripulante_elegido->id_tripulante);
    }

    //4. Movemos al tripulante elegido al punto de sabotaje.
    mover_tripulante_a_sabotaje(tripulante_elegido, pos_x,pos_y);
    miLogInfo("El tripulante: %d llego a la posicion de sabotaje",tripulante_elegido->id_tripulante);

    //5. Enviar FCSK a Store con posicion actual del tripulante.
    enviar_fcsk(tripulante_elegido->id_tripulante, tripulante_elegido->pos_x, tripulante_elegido->pos_y);


}