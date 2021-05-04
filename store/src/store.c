#include "store.h"

#define MODULE_NAME "I-Mongo-Store"
#define CONFIG_FILE_PATH "cfg/store.cfg"
#define LOG_FILE_PATH "store.log"

int main(int argc, char* argv[]) {

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició I-Mongo-Store.");

	if(leer_config()){
		miLogInfo("Error al iniciar I-Mongo-Store: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
	//Checkeo datos levantados del archivo de configuracion
	/*miLogInfo("Punto de montaje: %s", configuracion->puntoMontaje);
	miLogInfo("Puerto: %s", string_itoa(configuracion->puerto));
	miLogInfo("Tiempo sincronizacion: %s", string_itoa(configuracion->tiempoSincro));
	*/

	levantar_servidor(atender_request_store, string_itoa(configuracion->puerto));



	miLogInfo("Finalizó I-Mongo-Store.");
	miLogDestroy();
	free(configuracion);

	return EXIT_SUCCESS;
}


int leer_config(void){

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");

	config_destroy(config);
	return EXIT_SUCCESS;
}

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

	  default:
		miLogInfo("Me llego operacion: ...\n");

	  break;
	}
}