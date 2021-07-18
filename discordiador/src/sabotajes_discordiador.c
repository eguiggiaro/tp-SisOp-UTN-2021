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

		miLogInfo("Me llego operacion: ALERTA SABOTAJE en el punto: (%s)\n", posicion);

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

		break;

	default:
		miLogInfo("Me llego una operacion invalida\n");

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

void avisar_mov_sabotaje_bitacora(Tripulante* tripulante, char* origen, char* destino){

	t_paquete* paquete = crear_paquete(INFORMACION_BITACORA);
    t_buffer* buffer;
	char* id_trip = string_itoa(tripulante->id_tripulante);

	t_list* lista_mensajes = list_create();

	char* mensaje = string_new();
	string_append(&mensaje, "Se DESPLAZA el tripulante: ");
	string_append(&mensaje, id_trip);
	string_append(&mensaje, " desde: ");
	string_append(&mensaje, origen);
	string_append(&mensaje, " hasta: ");
	string_append(&mensaje, destino);

	//Se envia mensaje a Store:
	list_add(lista_mensajes,id_trip);
	list_add(lista_mensajes,mensaje);

	buffer = serializar_lista_strings(lista_mensajes);
    paquete ->buffer = buffer;
  
    enviar_paquete(paquete, tripulante->socket_store);

   //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(tripulante->socket_store);
	if (codigo_operacion == OK) {
		//miLogInfo("Nuevo desplazamiento informado a bitacora correctamente\n");
	} else if (codigo_operacion == FAIL){
        miLogError("Tripulante %d: ERROR INFORMANDO NUEVA POSICION A BITACORA. \n",tripulante->id_tripulante);
	}

	buffer = (t_buffer*)recibir_buffer(tripulante->socket_store);

	list_destroy(lista_mensajes);
	eliminar_buffer(buffer);
	free(mensaje);
}

void avisar_mov_sabotaje_miram(Tripulante* trip, char* eje){

	t_paquete* paquete = crear_paquete(MOV_TRIPULANTE);
    t_buffer* buffer;

	char* id_auxiliar = string_itoa(trip->id_tripulante);
	char* nueva_posicion;
	if(strncmp(eje,"X",1)==0){
		nueva_posicion = string_itoa(trip->pos_x);
	}
	else{
		nueva_posicion = string_itoa(trip->pos_y);
	}
	//Lista de elementos a enviar a miram: 1. ID del tripulante, 2. Eje, 3. Nueva Posicion
	t_list* lista_mensajes = list_create();
	list_add(lista_mensajes,id_auxiliar);
	list_add(lista_mensajes,eje);
	list_add(lista_mensajes,nueva_posicion);

	buffer = serializar_lista_strings(lista_mensajes);
    paquete ->buffer = buffer;
  
    enviar_paquete(paquete, trip->socket_miram);

   //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(trip->socket_miram);
	if (codigo_operacion == OK) {
		buffer = (t_buffer*)recibir_buffer(trip->socket_miram);
		eliminar_buffer(buffer);
		//miLogInfo("Nueva posicion informada a MIRAM correctamente\n");
	} else if (codigo_operacion == FAIL){
        miLogError("Tripulante %d: ERROR INFORMANDO NUEVA POSICION A MIRAM. \n",trip->id_tripulante);
	}

	list_destroy(lista_mensajes);
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

        sleep(configuracion->retardo_ciclo_cpu);

        miLogInfo("Se mueve el tripulante: %s desde: %s hasta: %s",id_trip,origen,destino);
        avisar_mov_sabotaje_bitacora(trip,origen,destino);
        //realizo movimiento
        trip->pos_x = (trip->pos_x)-1;
        //aviso a miram
        avisar_mov_sabotaje_miram(trip,"X");
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

        sleep(configuracion->retardo_ciclo_cpu);

        miLogInfo("Se mueve el tripulante: %s desde: %s hasta: %s \n",id_trip,origen,destino);
        avisar_mov_sabotaje_bitacora(trip,origen,destino);
        //realizo movimiento
        trip->pos_x = (trip->pos_x)+1;
        //aviso a miram
        avisar_mov_sabotaje_miram(trip,"X");
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

        sleep(configuracion->retardo_ciclo_cpu);

        miLogInfo("Se mueve el tripulante: %s desde: %s hasta: %s \n",id_trip,origen,destino);
        avisar_mov_sabotaje_bitacora(trip,origen,destino);
        //realizo movimiento
        trip->pos_y = (trip->pos_y)-1;
        //aviso a miram
        avisar_mov_sabotaje_miram(trip,"Y");
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

        sleep(configuracion->retardo_ciclo_cpu);

        miLogInfo("Se mueve el tripulante: %s desde: %s hasta: %s \n",id_trip,origen,destino);
        avisar_mov_sabotaje_bitacora(trip,origen,destino);
        //realizo movimiento
        trip->pos_y = (trip->pos_y)+1;
        //aviso a miram
        avisar_mov_sabotaje_miram(trip,"Y");
      }
    }
    else{
      distancia_y = 0;
    }
}

void enviar_fcsk(Tripulante* tripu){

	t_paquete* paquete = crear_paquete(FSCK);
  t_buffer* buffer;

  int id_tripulante = tripu->id_tripulante;
  
  int pos_x = tripu->pos_x; 
  int pos_y = tripu->pos_y;

	char* id_auxiliar = string_itoa(id_tripulante);
  char* pos_x_aux = string_itoa(pos_x);
  char* pos_y_aux = string_itoa(pos_y);

	t_list* lista_mensajes = list_create();
	list_add(lista_mensajes,id_auxiliar);
	list_add(lista_mensajes,pos_x_aux);
  list_add(lista_mensajes,pos_y_aux);

	buffer = serializar_lista_strings(lista_mensajes);
  paquete ->buffer = buffer;
  
  enviar_paquete(paquete, tripu->socket_store);

  //recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(tripu->socket_store);
	if (codigo_operacion == OK) {
		miLogInfo("FSCK informado correctamente\n");
	} else if (codigo_operacion == FAIL){
        miLogError("ERROR INFORMANDO FSCK. \n");
	}

	buffer = (t_buffer*)recibir_buffer(tripu->socket_store);

	list_destroy(lista_mensajes);
  eliminar_buffer(buffer);

}

void atender_sabotaje(char* posicion){
    char** array_posicion = string_split(posicion, ",");
    int pos_x = atoi(array_posicion[0]);
    int pos_y = atoi(array_posicion[1]);

    //1. Recorro lista general de tripulantes y los duermo a todos
    //TODO: agregar mutex
    /* 
    for(int i =0; i<list_size(tripulantes_totales);i++){

      Tripulante* tripu = list_get(tripulantes_totales,i);

      miLogInfo("Se duerme al tripulante: %d \n",tripu->id_tripulante);

      tripu->tripulante_despierto = false;
      //sem_wait(&(tripu->semaforo_trip)); GENERA DEADLOCK - REVISAR
    }
    
   if (pthread_create(&threadPAUSAR_PLANIFICACION, NULL, (void*) pausar_planificacion,
				NULL) != 0) {
			     printf("Error pausando planificacion\n");
		        }*/

    pausar_planificacion();
    //2.1 Recorro lista de EXEC y los paso a BLOCK_IO
    //TODO: ordenar por ID de menor a mayor
    //TODO: cambiar estado
    //pthread_mutex_lock(&mutexEXEC);
    /*if(list_size(execute_list) > 0){
      for(int i =0; i<list_size(execute_list);i++){

        Tripulante* tripu = list_remove(execute_list,i);
      
        list_add(blocked_em,tripu);
        tripu->estado_anterior = tripu->estado;
        tripu->estado = bloqueado_emergencia;

        miLogInfo("Se pasa de EXEC a BLOCK_EM al tripulante: %d , pos: %d|%d\n",tripu->id_tripulante, tripu->pos_x,tripu->pos_y);
      }
    } */
   // pthread_mutex_unlock(&mutexEXEC);

    //2.2 Recorro lista de READY (deberia chequear si no esta vacia?) y los paso a BLOCK_IO
    //pthread_mutex_lock(&mutexREADY); 
    /*if(list_size(ready_list)>0){
      for(int i =0; i<list_size(ready_list);i++){

        Tripulante* tripu = list_remove(ready_list,i);
      
        list_add(blocked_em,tripu);
        tripu->estado_anterior = tripu->estado;
        tripu->estado = bloqueado_emergencia;

        miLogInfo("Se pasa de READY a BLOCK_EM al tripulante: %d pos: %d|%d\n",tripu->id_tripulante,tripu->pos_x,tripu->pos_y);
      }
    }*/
   // pthread_mutex_unlock(&mutexREADY);
   for(int i = 0; i<list_size(tripulantes_totales); i++){
		  Tripulante* tripu = (Tripulante*) list_get(tripulantes_totales,i);
      if(tripu->estado==trabajando){
        tripu->estado_anterior = tripu->estado;
        tripu->estado = bloqueado_emergencia;
        miLogInfo("Se pasa de EXEC a BLOCK_EM al tripulante: %d , pos: %d|%d\n",tripu->id_tripulante, tripu->pos_x,tripu->pos_y);
      }
      else if(tripu->estado==listo){
        tripu->estado_anterior = tripu->estado;
        tripu->estado = bloqueado_emergencia;
        miLogInfo("Se pasa de READY a BLOCK_EM al tripulante: %d pos: %d|%d\n",tripu->id_tripulante,tripu->pos_x,tripu->pos_y);
      }
      else if(tripu->estado==bloqueado_io){
        tripu->en_sabotaje = true;
        miLogInfo("Se frena consumo de ciclos IO para tripulante: %d\n",tripu->id_tripulante);
      }
				
		}
    list_add_all(blocked_em,execute_list);
    list_add_all(blocked_em,ready_list);

    //3. Calculo al tripulante en la posicion mas cercana y lo paso a cola de BLOCK_EMERGENCIA
    void* tripulante_mas_cercano(Tripulante* un_tripu, Tripulante* otro_tripu){

      double primera_distancia = raiz_cuadrada( (pos_x - un_tripu->pos_x)*(pos_x - un_tripu->pos_x) +
      (pos_y - un_tripu->pos_y)*(pos_y - un_tripu->pos_y) );

      double segunda_distancia = raiz_cuadrada( (pos_x - otro_tripu->pos_x)*(pos_x - otro_tripu->pos_x) +
      (pos_y - otro_tripu->pos_y)*(pos_y - otro_tripu->pos_y) );

      return primera_distancia <= segunda_distancia ? un_tripu : otro_tripu;
    }

    Tripulante* tripulante_elegido = (Tripulante*) list_get_minimum(blocked_em,(void*)tripulante_mas_cercano);

    //tripulante_elegido->tripulante_despierto = true;

    miLogInfo("El tripulante elegido para llevar a cabo el sabotaje es el: %d \n",tripulante_elegido->id_tripulante);

    //3.1 Sacamos al tripulante de la cola de block emergencia
    for(int i =0; i<list_size(blocked_em);i++){
      Tripulante* trip_aux = list_get(blocked_em,i);
      if(trip_aux->id_tripulante == tripulante_elegido->id_tripulante){
        list_remove(blocked_em,i);
        break;
      }
    }

    //4. Movemos al tripulante elegido al punto de sabotaje.
    mover_tripulante_a_sabotaje(tripulante_elegido, pos_x,pos_y);
    miLogInfo("El tripulante: %d llego a la posicion de sabotaje: %d|%d \n",tripulante_elegido->id_tripulante,
                                                                            tripulante_elegido->pos_x,
                                                                            tripulante_elegido->pos_y);

    //5. Enviar FCSK a Store con posicion actual del tripulante.
    enviar_fcsk(tripulante_elegido);

    //6. Esperar tiempo de duracion de tarea.
    int retardo = configuracion->retardo_ciclo_cpu;
    int ciclos_sabotaje = configuracion->duracion_sabotaje;
    sleep(ciclos_sabotaje*retardo);

    //7.Pasar tripulante elegido a ultimo lugar en cola de BLOCK_EMERGENCIA
    list_add(blocked_em,tripulante_elegido);

    //8. Desbloquear y despertar a todos los tripulantes. (previamente, guardar estado anterior)
    for(int i = 0; i<list_size(tripulantes_totales); i++){
		  Tripulante* tripu = (Tripulante*) list_get(tripulantes_totales,i);
      if(tripu->estado_anterior==trabajando){
        tripu->estado = trabajando;
        tripu->estado_anterior = bloqueado_emergencia;
        miLogInfo("Se pasa de BLOCK_EM a EXEC al tripulante: %d , pos: %d|%d\n",tripu->id_tripulante, tripu->pos_x,tripu->pos_y);
        //pthread_mutex_unlock(&tripu->semaforo_trip);
      }
      else if(tripu->estado_anterior==listo){
        tripu->estado = listo;
        tripu->estado_anterior = bloqueado_emergencia;
        miLogInfo("Se pasa de BLOCK_EM a READY al tripulante: %d pos: %d|%d\n",tripu->id_tripulante,tripu->pos_x,tripu->pos_y);
        
      }
      else if(tripu->estado==bloqueado_io){
        tripu->en_sabotaje = false;
        miLogInfo("Se reanuda consumo de ciclos IO para tripulante: %d\n",tripu->id_tripulante);
        pthread_mutex_unlock(&tripu->semaforo_trip);
      }
				
		}

    list_clean(blocked_em);

    if (pthread_create(&threadINICIAR_PLANIFICACION, NULL, (void*) iniciar_planificacion,
				NULL) != 0) {
			     printf("Error iniciando planificacion/n");
		}

    pthread_exit(0);

}