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

		int pos_X = atoi(list_get(lista, 0));
        int pos_Y = atoi(list_get(lista, 1));

		miLogInfo("\nMe llego operacion: ALERTA SABOTAJE en el punto: (%d, %d)\n", pos_X,pos_Y);

        t_paquete *paquete_devuelto = crear_paquete(OK);
		
        list_add(lista_mensajes, "Operacion recibida correctamente");

		buffer = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer;
		enviar_paquete(paquete_devuelto, request_fd);
		list_destroy(lista_mensajes);
		list_destroy(lista);
		//eliminar_paquete(paquete_devuelto);
		free(request);

		break;

	default:
		miLogInfo("\nMe llego una operacion invalida\n");

		break;
	}
}