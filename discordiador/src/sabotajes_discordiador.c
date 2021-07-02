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
    sem_wait(&mutexEXEC);
    if(list_size(execute_list) > 0){
    for(int i =0; i<list_size(execute_list);i++){

      Tripulante* tripu = list_remove(execute_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de EXEC a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    sem_post(&mutexEXEC);

    //2.2 Recorro lista de READY (deberia chequear si no esta vacia?) y los paso a BLOCK_IO
    sem_wait(&mutexREADY); 
    if(list_size(ready_list)>0){
    for(int i =0; i<list_size(ready_list);i++){

      Tripulante* tripu = list_remove(ready_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de READY a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    sem_post(&mutexREADY);

    //2.3 Recorro lista de NEW (deberia chequear si no esta vacia?) y los paso a BLOCK_IO
    sem_wait(&mutexNEW);
    if(list_size(new_list)>0){ 
    for(int i =0; i<list_size(new_list);i++){

      Tripulante* tripu = list_remove(new_list,i);
      
      list_add(blocked_io,tripu);

      miLogInfo("\nSe pasa de NEW a BLOCK al tripulante: %d \n",tripu->id_tripulante);
    }
    }
    sem_post(&mutexNEW);

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
}