#include "discordiador.h"

#define MODULE_NAME "Discordiador"
#define CONFIG_FILE_PATH "cfg/discordiador.cfg"
#define LOG_FILE_PATH "cfg/discordiador.log"

int main(){

	
	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, false, LOG_LEVEL_INFO);
	miLogInfo("Inició Discordiador.");

	if(leer_config()){
		miLogInfo("Error al iniciar Discordiador: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
		
	//Obtención datos para conexion con miram
	char* ip_miram = configuracion->ip_mi_ram_hq;
	char* puerto_miram = configuracion->puerto_mi_ram_hq ;


	miLogInfo("Conectándose a MiRAM");
	iniciar_conexion(ip_miram, puerto_miram);

	//Obtención datos para conexion con store
	char* ip_store = configuracion->ip_i_mongo_store;
	char* puerto_store = configuracion->puerto_i_mongo_store ;


	miLogInfo("Conectándose a Store");
	iniciar_conexion(ip_store, puerto_store);
	
	miLogInfo("Finalizó Discordiador");
	free(configuracion);
	miLogDestroy();

}

void iniciar_conexion(char* ip_destino, char* puerto_destino) {

//inicia conexion con destino
    int socket = crear_conexion(logger, ip_destino, puerto_destino);

	miLogInfo("Obtuve el socket y vale %d.\n", socket);

	if (socket == -1) {
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
    
	//envia mensaje a destino
    t_paquete* paquete = crear_paquete(PAQUETE);
	t_buffer* buffer;
	t_list* lista_mensajes = list_create();

	list_add(lista_mensajes, "hola");
    list_add(lista_mensajes,"soy discordiador!");

	buffer = serializar_lista_strings(lista_mensajes);
	paquete ->buffer = buffer;
	enviar_paquete(paquete, socket);

	//recibe respuesta de destino
	op_code codigo_operacion = recibir_operacion(socket);
	if (codigo_operacion == OK) {

		t_buffer* buffer = recibir_buffer(socket);
		t_list* lista = deserializar_lista_strings(buffer);

		//loggear_lista_strings(lista);
		list_iterate(lista, printf);

		miLogInfo("Recibi los mensajes del destino correctamente");
	} else {
		miLogInfo("No recibi los mensajes del destino correctamente");
	}
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
	configuracion->ip_mi_ram_hq = strdup(config_get_string_value(config, "IP_MI_RAM_HQ"));
	configuracion->puerto_mi_ram_hq = strdup(config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
	configuracion->ip_i_mongo_store = strdup(config_get_string_value(config, "IP_I_MONGO_STORE"));
	configuracion->puerto_i_mongo_store = config_get_int_value(config, "PUERTO_I_MONGO_STORE");
	configuracion->grado_multitarea = config_get_int_value(config, "GRADO_MULTITAREA");
	configuracion->algoritmo = strdup(config_get_string_value(config, "ALGORITMO"));
	configuracion->quantum = config_get_int_value(config, "QUANTUM");
	configuracion->duracion_sabotaje = config_get_int_value(config, "DURACION_SABOTAJE");
	configuracion->retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	
	config_destroy(config);
	return EXIT_SUCCESS;
}



