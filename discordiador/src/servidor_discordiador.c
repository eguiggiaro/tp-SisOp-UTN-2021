#include "servidor_discordiador.h"

void* iniciar_servidor_discordiador(){
    levantar_servidor(atender_request_discordiador,puerto_discordiador);
	return NULL;
}

void atender_request_discordiador(int request_fd){
	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_devolucion;
	t_list* lista;
	t_paquete* paquete_devuelto;
	t_list* lista_mensajes;


	switch(codigo_operacion){
		
	  case ALERTA_SABOTAJE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: ALERTA SABOTAJE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy discordiador");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  list_destroy(lista);
	  list_destroy(lista_mensajes);
	  eliminar_buffer(buffer_devolucion);

	  break;

	  default:
		miLogInfo("Me llego operacion: ...\n");

	  break;
	}
}