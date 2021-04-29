#include "miram.h"

int main(){

    t_log* logger = log_create("./cfg/miram.log", "MiRAM", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy MiRAM! %s", mi_funcion_compartida());

	//Obtención datos para conexion con discordiador
	//estos datos deberian obtenerse del archivo config
	char* ip_discordiador = "127.0.0.1";
	char* puerto_discordiador = "5002"; 
	log_info(logger, "IP discordiador: %s\n", ip_discordiador);
	log_info(logger, "Puerto discordiador: %s\n", puerto_discordiador);
    
	//inicia conexion con discordiador
    int socket_discor = crear_conexion(ip_discordiador, puerto_discordiador);
	log_info(logger, "Socket vale %d.\n", socket_discor);
	if (socket_discor == -1) {
		printf("Algo ha salido mal. Por favor intente de nuevo más tarde.\n"); 
		log_error(logger,
				"No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
    
	//envia mensaje a discordiador
    t_paquete* paquete_discordiador = crear_paquete_con_operacion(PAQUETE);
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

		loggear_lista_strings(lista, logger);

		log_info(logger, "Recibi los mensajes del discordiador correctamente");
	} else {
		log_info(logger, "No recibi los mensajes del discordiador correctamente");
	}

    log_destroy(logger);
}