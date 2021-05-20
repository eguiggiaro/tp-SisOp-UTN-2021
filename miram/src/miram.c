#include "miram.h"

#define MODULE_NAME "MiRAM"
#define CONFIG_FILE_PATH "cfg/miram.cfg"
#define LOG_FILE_PATH "miram.log"


int leer_config(void) {

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	configuracion->esquema_memoria = strdup(config_get_string_value(config, "ESQUEMA_MEMORIA"));
	configuracion->tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	configuracion->tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
	configuracion->path_swap = strdup(config_get_string_value(config, "PATH_SWAP"));
	configuracion->algoritmo_reemplazo = strdup(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	configuracion->algoritmo_busqueda = strdup(config_get_string_value(config, "ALGORITMO_BUSQUEDA"));
	config_destroy(config);
	return EXIT_SUCCESS;
}

void atender_request_miram(uint32_t request_fd) {

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_devolucion;
	t_list* lista;
	t_paquete* paquete_devuelto;
	t_list* lista_mensajes;


	switch(codigo_operacion){
		
	  case EXPULSAR_TRIPULANTE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: EXPULSAR TRIPULANTE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy miram");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;

	  case INICIAR_TRIPULANTE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: INICIAR TRIPULANTE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy miram");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;

	  case INFORMAR_TAREAS_PATOTA:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: INFORMAR TAREAS PATOTA \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy miram");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;

	  case TAREA_SIGUIENTE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: TAREA SIGUIENTE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy miram");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;

	  case MOV_TRIPULANTE:
	  //recibo los mensajes
	  miLogInfo("Me llego operacion: MOVER TRIPULANTE \n");
	  buffer_devolucion = recibir_buffer(request_fd);

	  lista = deserializar_lista_strings(buffer_devolucion);
	  loggear_lista_strings(lista);

	  //devuelve una lista de mensajes
	  paquete_devuelto = crear_paquete(OK);

	  lista_mensajes = list_create();
	  list_add(lista_mensajes, "hola");
      list_add(lista_mensajes,"soy miram");


	  buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  paquete_devuelto->buffer = buffer_devolucion;
	  enviar_paquete(paquete_devuelto, request_fd);

	  break;

	  default:
		miLogInfo("Me llego operacion: ...\n");

	  break;
	}
}

void crear_grilla(void) {
	NIVEL* nivel;

	int cols, rows;
	int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("Test");

	err = personaje_crear(nivel, 'A', cols - 1, rows - 1);
	err = enemigo_crear(nivel, 'B', 10, 14);
	err = caja_crear(nivel, 'X', 19, 9, 2);
	nivel_gui_dibujar(nivel);
    sleep(3);
	nivel_destruir(nivel);
	nivel_gui_terminar();
	
}

void inicializar_memoria(int tamanio_memoria)
{
	MEMORIA = malloc(tamanio_memoria);
	contador_patotas = 0;
	contador_tripulantes = 0;

	if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		inicializar_segmentacion(tamanio_memoria);
	}

}

void finalizar_memoria()
{

	if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		finalizar_segmentacion();
	}

	free(MEMORIA);

}

int reservar_memoria(int bytes)
{
		if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		if (configuracion->algoritmo_busqueda = "FF")
		{
			return reservar_memoria_segmentacion_ff(bytes);
		}
	}
}

int iniciar_patota(int cantidad_tripulantes, Tarea *tareas, Punto *puntos) 
{
	u_int32_t posicion_memoria = reservar_memoria(sizeof(PCB));
	
	if (posicion_memoria == 99)
	{
		return -1;
	}
	
	// Seccion agregar tareas

	// Fin seccion agregar tareas

	PCB* unPCB = posicion_memoria;

	// Sincronizar
	unPCB->PID = contador_patotas++;
	unPCB->Tareas = 1000;

}


int iniciar_tripulante(int patota, Punto* unPunto) 
{
	u_int32_t posicion_memoria = reservar_memoria(sizeof(PCB));

	if (posicion_memoria == 99)
	{
		return -1;
	}

	TCB* miTCB = reservar_memoria(sizeof(TCB));

	//sincronizar
	miTCB->TID = contador_tripulantes++;

	miTCB->estado = 'N';
	miTCB->pos_X = unPunto->pos_x;
	miTCB->pos_y = unPunto->pos_y;

	//linkear a tareas
	miTCB->proxima_instruccion = 1212;
	miTCB->PCB = 1212;

}



void hacer_memoria(int tamanio_memoria)
{

/*
	TCB* miTCB = malloc(sizeof(TCB));
	miTCB->TID = 100;
	miTCB->estado = 'E';
	miTCB->pos_X = 15;
	miTCB->pos_y = 20;
	miTCB->proxima_instruccion = 1212;
	miTCB->PCB = 1212;

	mostrar_tabla_segmentos(true);
*/

	
//  
	//SERVICIOS A CONSTRUIR
	// 1- NUEVO ELEMENTO:
	// 2- Crear un segmento
	// 3- Eliminar un segmento
	// ¡cómo sé que llegué al final de la memoria?

	// si direccion es mayor a base + limite o menor a base, error


// list_add(t_list *self, void *data) {
//void list_iterate(t_list* self, void(*closure)(void*)) {
//void* list_find(t_list *self, bool(*condition)(void*)) {


//static t_link_element* list_find_element(t_list *self, bool(*cutting_condition)(t_link_element*, int)) {
//	t_link_element* element = self->head;
//	int index = 0;

//	while(!cutting_condition(element, index)) {
//		element = element->next;
//		index++;
//	}

//	return element;
//}
}

void* iniciar_servidor_miram(){
    levantar_servidor(atender_request_miram,puerto_miram);
	return NULL;
}

void* iniciar_funciones_memoria(){
    inicializar_memoria(tamanio_memoria);
	mostrar_tabla_segmentos(true);
	hacer_memoria(tamanio_memoria);
	return NULL;
}

int main(){

	//pthread_t mapa;
	//pthread_create(&mapa, NULL, (void*)crear_grilla, NULL);
	
   //Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició MiRAM.");

	if(leer_config()){
		miLogInfo("Error al iniciar MiRAM: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	tamanio_memoria = configuracion->tamanio_memoria;

	puerto_miram = string_itoa(configuracion->puerto);

	if (pthread_create(&threadSERVER, NULL, (void*) iniciar_servidor_miram,
			NULL) != 0) {
		printf("Error iniciando servidor/n");
	}

	if (pthread_create(&threadMAPA, NULL, (void*) crear_grilla,
			NULL) != 0) {
		printf("Error creando grilla/n");
	}

	if (pthread_create(&threadMEMORIA, NULL, (void*) iniciar_funciones_memoria,
			NULL) != 0) {
		printf("Error iniciando memoria/n");
	}


    pthread_join(threadSERVER, NULL);
	pthread_join(threadMAPA, NULL);
	pthread_join(threadMEMORIA, NULL);

	miLogInfo("Finalizó MiRAM");
	free(configuracion);
	finalizar_memoria();
    miLogDestroy();


}
