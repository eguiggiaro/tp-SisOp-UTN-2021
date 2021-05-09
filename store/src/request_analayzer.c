#include "request_analyzer.h"

void atender_request_store(uint32_t request_fd) {

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_devolucion;

	switch(codigo_operacion){
		
	  case PAQUETE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: PAQUETE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  t_list* lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  t_paquete* paquete_devuelto = crear_paquete(OK);

	  t_list* lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes, "soy store");

	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;
	
	  case MENSAJE:
		miLogInfo("Me llego operacion: MENSAJE\n");

	  break;

	  case GENERAR_OXIGENO:
	  	/*TODO: 
		Verificar que exista un archivo llamado Oxigeno.ims en el i-Mongo-Store
		Si existe saltar al paso 4.
		Si no existe el archivo, crearlo y asignarle el carácter de llenado O
		Agregar tantos caracteres de llenado del archivo como indique el parámetro CANTIDAD
		*/
		miLogInfo("Me llego operacion: GENERAR_OXIGENO\n");

	  break;
			
	  default:
		miLogInfo("Me llego operacion: ...\n");

	  break;
	}
}