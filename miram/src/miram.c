#include "miram.h"

#define MODULE_NAME "MiRAM"
#define CONFIG_FILE_PATH "cfg/miram.cfg"
#define LOG_FILE_PATH "miram.log"



// Lee la configuración y la deja disponible
int leer_config(void) {

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}


	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	configuracion->esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	configuracion->tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	configuracion->tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
	configuracion->path_swap = config_get_string_value(config, "PATH_SWAP");
	configuracion->algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	configuracion->algoritmo_busqueda = config_get_string_value(config, "ALGORITMO_BUSQUEDA");
	config_destroy(config);
	return EXIT_SUCCESS;
}

// Atiende con hilos los request que son enviados a MiRAM
void atender_request_miram(uint32_t request_fd) {

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_devolucion;
	t_list* lista;
	t_paquete* paquete_devuelto;
	t_list* lista_mensajes;
	int resultado;

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

			int PATOTA_ID =  atoi(list_get(lista,0));
			
			resultado = iniciar_tripulante(PATOTA_ID);

			if (resultado == -1)
			{
				miLogInfo("ERROR: TRIPULANTE NO INICIADO \n");
				paquete_devuelto = crear_paquete(FAIL);
				list_add(lista_mensajes, "Se produjo un error al iniciar el tripulante");
			} else {
				miLogInfo("TRIPULANTE INICIADO CORRECTAMENTE \n");

				char* posicion = buscar_posicion_tripulante(resultado);
				char* proxima_tarea = proxima_tarea_tripulante(resultado);

				paquete_devuelto = crear_paquete(OK);
				list_add(lista_mensajes, string_itoa(resultado));
				list_add(lista_mensajes, posicion);
				list_add(lista_mensajes, proxima_tarea);
			}

				buffer_devolucion = serializar_lista_strings(lista_mensajes);
				paquete_devuelto->buffer = buffer_devolucion;
				enviar_paquete(paquete_devuelto, request_fd);

			break;

	  case INICIAR_PATOTA:
			//recibo los mensajes
			miLogInfo("Me llego operacion: INICIAR PATOTA \n");
			buffer_devolucion = recibir_buffer(request_fd);

			lista = deserializar_lista_strings(buffer_devolucion);

			int cantidad_tripulantes =  atoi(list_get(lista,0));
			char* tareas = list_get(lista,1);

			Punto* unPunto = malloc(sizeof(Punto));
			unPunto->pos_x = 10;
			unPunto->pos_y = 15;

			Punto* unPunto2 = malloc(sizeof(Punto));
			unPunto2->pos_x = 100;
			unPunto2->pos_y = 125;

			Punto puntos[2] = {*unPunto,*unPunto2};
			lista_mensajes = list_create();
			resultado = iniciar_patota(cantidad_tripulantes,tareas,puntos);

			if (resultado == -1)
			{
				miLogInfo("ERROR: PATOTA NO INICIADA \n");

				paquete_devuelto = crear_paquete(FAIL);
				list_add(lista_mensajes, "Se produjo un error al iniciar la patota");
			} else {
				miLogInfo("PATOTA INICIADA CORRECTAMENTE \n");

				paquete_devuelto = crear_paquete(OK);
				list_add(lista_mensajes, string_itoa(resultado));
			}

			buffer_devolucion = serializar_lista_strings(lista_mensajes);
			paquete_devuelto->buffer = buffer_devolucion;

			miLogInfo("Enviando paquete de vuelta \n");


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

// Crea la grilla inicial y la muestra en pantalla
void crear_grilla(void) {

	int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("AMONGOS");

	err = personaje_crear(nivel, 'A', 0,0);
	err = personaje_crear(nivel, 'B', 3,3);
	nivel_gui_dibujar(nivel);
}

void crear_personaje_grilla(int tripulante, int pos_x, int pos_y) {
	char identificador = identificadores[proximo_identificador++];
	
	Identidad_grilla* id_grilla = malloc(sizeof(Identidad_grilla));
	id_grilla->TID = tripulante;
	id_grilla->identificador = identificador;
	
	list_add(tabla_identificadores_grilla, id_grilla);
	personaje_crear(nivel, identificador, pos_x,pos_y);
	nivel_gui_dibujar(nivel);
}

//Mueve un tripulante a una dirección destino
int mover_tripulante(int tripulante, int posicion_x_final, int posicion_y_final)
{
	TCB* miTCB = buscar_tripulante(tripulante);

	if (miTCB = 99) {
		return -1;
	}

	char identificador = buscar_tripulante_grilla(tripulante);

	if (identificador == '-') {
		return -1;
	}

	mover_tripulante_grilla(identificador, posicion_x_final - miTCB->pos_X, posicion_y_final - miTCB->pos_y);
}

void mover_tripulante_grilla(char identificador, int pos_x, int pos_y) {

	int err;

	err = item_desplazar(nivel, identificador, pos_x, pos_y);
	nivel_gui_dibujar(nivel);

}

//buscar el caracter asignado en la grilla, para el tripulante
char buscar_tripulante_grilla(int tripulante)
{

	if (tripulante > contador_patotas) {
		miLogInfo("El tripulante %d a mover no existe", tripulante);
		return '-';
	}

	bool encontre_tripulante = false;
	char identificador_tripulante;
	TCB* unTCB;
	TCB* TCB_encontrado;

	t_list_iterator* list_iterator = list_iterator_create(tabla_identificadores_grilla);
                Identidad_grilla* id_grilla;

				while(list_iterator_has_next(list_iterator)) {
                    id_grilla = list_iterator_next(list_iterator);
					
					if (id_grilla->TID == tripulante)
					{
						encontre_tripulante = true;
						identificador_tripulante = id_grilla->identificador;
						break;
					}
                }
	list_iterator_destroy(list_iterator);

	if (encontre_tripulante)
	{
		return identificador_tripulante;
	} else {
		miLogInfo("El tripulante %d a mover no existe", tripulante);
		return '-';
	}


}



void inicializar_memoria(int tamanio_memoria)
{
	MEMORIA = malloc(tamanio_memoria);
	contador_patotas = 0;
	contador_tripulantes = 0;
	tabla_identificadores_grilla = list_create();

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
	nivel_destruir(nivel);
	nivel_gui_terminar();

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

int alta_patota(PCB* unPCB)
{
		if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return alta_patota_segmentacion(unPCB);
	}
}

u_int32_t buscar_patota(int PCB_ID)
{
	if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return buscar_patota_segmentacion(PCB_ID);
	}
}

void alta_tripulante(TCB* unTCB)
{
		if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return alta_tripulante_segmentacion(unTCB);
	}
}

void alta_tareas(int PCB_ID, char* tareas)
{
		if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return alta_tareas_segmentacion(PCB_ID, tareas);
	}
}

u_int32_t buscar_tripulante(int TCB_ID)
{
	if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return buscar_tripulante_segmentacion(TCB_ID);
	}
}


u_int32_t buscar_tripulante_no_asignado(int PCB_ID)
{
	if (configuracion->esquema_memoria = "SEGMENTACION")
	{
		return buscar_tripulante_no_asignado_segmentacion(PCB_ID);
	}
}


u_int32_t iniciar_tareas(int PCB_ID, char* tareas) 
{
	u_int32_t posicion_memoria = reservar_memoria(strlen(tareas));

	if (posicion_memoria == 99)
	{
		return 99;
	}

	memcpy(posicion_memoria, tareas,sizeof(tareas));

	alta_tareas(PCB_ID,posicion_memoria);

	return posicion_memoria;
}

char* buscar_posicion_tripulante(int tripulante_id) {

	u_int32_t posicion_memoria = buscar_tripulante(tripulante_id);
	
	if (posicion_memoria == 99)
	{
		return "";
	}

	TCB* unTCB = posicion_memoria;
	char* posicion_tripulante = string_duplicate(string_itoa(unTCB->pos_X));
	
	posicion_tripulante = (posicion_tripulante,'|');
	posicion_tripulante = (posicion_tripulante,string_itoa(unTCB->pos_y));

	return posicion_tripulante;
}

char* proxima_tarea_tripulante(int tripulante_id) {

	u_int32_t posicion_memoria = buscar_tripulante(tripulante_id);
	
	if (posicion_memoria == 99)
	{
		return "";
	}

	TCB* unTCB = posicion_memoria;

	//TODO

	return "";
}



int iniciar_patota(int cantidad_tripulantes, char* tareas, Punto *puntos) 
{
	miLogInfo("Iniciando patota \n");
	u_int32_t posicion_memoria = reservar_memoria(sizeof(PCB));
	
	if (posicion_memoria == 99)
	{
		return -1;
	}
	
	PCB* unPCB = posicion_memoria;
	// Sincronizar
	unPCB->PID = contador_patotas++;

	miLogInfo("Iniciando tareas \n");

	posicion_memoria = iniciar_tareas(unPCB->PID, tareas);

	if (posicion_memoria == 99)
	{
		return -1;
	}

	unPCB->Tareas = posicion_memoria;

	//alta patota
	alta_patota(unPCB);

	miLogInfo("Iniciando tripulantes \n");

	for (int i=0; i<cantidad_tripulantes;i++)
	{
		if (inicializar_tripulante(unPCB->PID, puntos[i], unPCB->Tareas) == -1)
		{
			return -1;
		}
		
	}
	return unPCB->PID;
}



int inicializar_tripulante(int patota, Punto unPunto, u_int32_t tareas) 
{
	u_int32_t posicion_memoria = reservar_memoria(sizeof(TCB));

	if (posicion_memoria == 99)
	{
		return -1;
	}

	TCB* miTCB = posicion_memoria;

	//sincronizar
	miTCB->TID = contador_tripulantes++;

	miTCB->estado = 'N';
	miTCB->pos_X = (&unPunto)->pos_x;
	miTCB->pos_y = (&unPunto)->pos_y;

	//linkear a tareas
	miTCB->proxima_instruccion = tareas;
	miTCB->PCB = buscar_patota(patota);

	//alta tripulante
	alta_tripulante(miTCB);

}

int iniciar_tripulante(int patota_id) 
{
	u_int32_t posicion_memoria = buscar_tripulante_no_asignado(patota_id);

	if (posicion_memoria == 99)
	{
		return -1;
	}

	TCB* unTCB = posicion_memoria;
	unTCB->estado = 'R';

	return unTCB->TID;
}



void hacer_memoria(int tamanio_memoria)
{
	/*
	Punto* unPunto = malloc(sizeof(Punto));
	unPunto->pos_x = 10;
	unPunto->pos_y = 15;

	Punto* unPunto2 = malloc(sizeof(Punto));
	unPunto2->pos_x = 100;
	unPunto2->pos_y = 125;

	Punto puntos[2] = {*unPunto,*unPunto2};

	char* tareas = "HACER_ALGO 5;5;6;8|HACER_OTRACOSA;5;6;8";

	int IDPATOTA = iniciar_patota(2,tareas,puntos);

	PCB* unPCB = buscar_patota(IDPATOTA);
	char* tareas1 = unPCB->Tareas;

	TCB* unTCB = buscar_tripulante(0);

	free(unPunto);
	free(unPunto2);
*/
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
	hacer_memoria(tamanio_memoria);
	mostrar_tabla_segmentos(true);
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
	iniciar_funciones_memoria();
	iniciar_servidor_miram();


/*
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
*/

    //pthread_join(threadSERVER, NULL);
	//pthread_join(threadMAPA, NULL);
	//pthread_join(threadMEMORIA, NULL);

	miLogInfo("Finalizó MiRAM");
	free(configuracion);
	finalizar_memoria();
    miLogDestroy();


}
