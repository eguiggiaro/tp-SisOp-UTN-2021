#include "discordiador.h"

#define MODULE_NAME "Discordiador"
#define CONFIG_FILE_PATH "cfg/discordiador.cfg"
#define LOG_FILE_PATH "discordiador.log"

int main(){

	char* puerto_discordiador = NULL;
	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició Discordiador.");

	if(leer_config()){
		miLogInfo("Error al iniciar Discordiador: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
	
	puerto_discordiador = string_itoa(configuracion->puerto);

  	levantar_servidor(atender_request_discordiador,puerto_discordiador);


	miLogInfo("Finalizó Discordiador");
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



void atender_request_discordiador(uint32_t request_fd){

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_miram;

	switch(codigo_operacion){
		
	  case PAQUETE:
	  //recibo los mensajes de miram
	  miLogInfo("Me llego operacion: PAQUETE \n");
	  buffer_miram = recibir_buffer(request_fd);

	  t_list* lista = deserializar_lista_strings(buffer_miram);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes a miram
	  t_paquete* paquete_miram = crear_paquete(OK);

	  t_list* lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy el discordiador");


	  buffer_miram = serializar_lista_strings(lista_mensajes);
	  paquete_miram->buffer = buffer_miram;
	  enviar_paquete(paquete_miram, request_fd);

	  break;
	
	  case MENSAJE:
		miLogInfo("Me llego operacion: MENSAJE\n");

	  break;

	  default:
		miLogInfo("Me llego operacion: ...\n");

	  break;
	}

}