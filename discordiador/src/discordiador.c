#include "discordiador.h"

int main(){
  logger = log_create("./cfg/discordiador.log", "Discordiador", true, LOG_LEVEL_INFO);
  log_info(logger, "Soy el discordiador desde utils! %s", mi_funcion_compartida());

  char* puerto_discordiador = "5002";

  iniciar_servidor(atender_request_discordiador,puerto_discordiador);

  log_destroy(logger);
}

void atender_request_discordiador(uint32_t request_fd){

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_miram;

	switch(codigo_operacion){
	  case PAQUETE:
	  //recibo los mensajes de miram
	  log_info(logger, "Me llego operacion: PAQUETE \n");

	  buffer_miram = recibir_buffer(request_fd);

	  t_list* lista = deserializar_lista_strings(buffer_miram);
	  loggear_lista_strings(lista, logger);

	  //devuelve una lista de mensajes a miram
	  t_paquete* paquete_miram = crear_paquete_con_operacion(OK);

	  t_list* lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy el discordiador");


	  buffer_miram = serializar_lista_strings(lista_mensajes);
	  paquete_miram->buffer = buffer_miram;
	  enviar_paquete(paquete_miram, request_fd);

	  break;
	
	  case MENSAJE:

		log_info(logger, "Me llego operacion: MENSAJE\n");

	  break;

	  default:

		log_info(logger, "Me llego operacion: ...\n");

	  break;
	}
}