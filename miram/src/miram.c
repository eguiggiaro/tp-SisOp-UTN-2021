#include "miram.h"

#define MODULE_NAME "MiRAM"
#define CONFIG_FILE_PATH "cfg/miram.cfg"
#define LOG_FILE_PATH "miram.log"

int main(){

	char* puerto_miram = NULL;
   //Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició MiRAM.");

	if(leer_config()){
		miLogInfo("Error al iniciar MiRAM: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	puerto_miram = string_itoa(configuracion->puerto);

	levantar_servidor(atender_request_miram,puerto_miram);


	miLogInfo("Finalizó MiRAM");
	free(configuracion);
    miLogDestroy();
}

int leer_config(void) {

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

void atender_request_miram(uint32_t request_fd) {

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
      list_add(lista_mensajes,"soy miram");


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