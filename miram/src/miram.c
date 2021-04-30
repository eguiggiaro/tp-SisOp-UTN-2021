#include "miram.h"

#define MODULE_NAME "MiRAM"
#define CONFIG_FILE_PATH "cfg/miram.cfg"
#define LOG_FILE_PATH "miram.log"

int main(){

   //Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició MiRAM.");

	if(leer_config()){
		miLogInfo("Error al iniciar MiRAM: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	//Obtención datos para conexion con discordiador
	//estos datos deberian obtenerse del archivo config
	char* ip_discordiador = "127.0.0.1";
	char* puerto_discordiador = "5002"; 


	printf("IP discordiador: %s\n", ip_discordiador);
	printf("Puerto discordiador: %s\n", puerto_discordiador);
    
	//inicia conexion con discordiador
    int socket_discor = crear_conexion(ip_discordiador, puerto_discordiador);
	printf("Socket vale %d.\n", socket_discor);
	if (socket_discor == -1) {
		printf("Algo ha salido mal. Por favor intente de nuevo más tarde.\n"); 
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
    
	//envia mensaje a discordiador
    t_paquete* paquete_discordiador = crear_paquete(PAQUETE);
	t_buffer* buffer_discordiador;
	t_list* lista_mensajes = list_create();

	list_add(lista_mensajes, "hola");
    list_add(lista_mensajes,"soy miram!");

	buffer_discordiador = serializar_lista_strings(lista_mensajes);
	paquete_discordiador->buffer = buffer_discordiador;
	enviar_paquete(paquete_discordiador, socket_discor);

	//recibe respuesta del discordiador
	op_code codigo_operacion = recibir_operacion(socket_discor);
	if (codigo_operacion == OK) {

		t_buffer* buffer = recibir_buffer(socket_discor);
		t_list* lista = deserializar_lista_strings(buffer);

		loggear_lista_strings(lista);

		miLogInfo("Recibi los mensajes del discordiador correctamente");
	} else {
		miLogInfo("No recibi los mensajes del discordiador correctamente");
	}

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