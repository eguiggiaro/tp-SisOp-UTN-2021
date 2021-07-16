#include "miram.h"

#define MODULE_NAME "MiRAM"
#define CONFIG_FILE_PATH "cfg/miram.cfg"
#define LOG_FILE_PATH "cfg/miram.log"

void compactar_memoria(void)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		miLogInfo("Compactando!");
		iniciar_servidor_miram();
	}
	else
	{
		miLogInfo("No se puede compactar en paginación!");
	}
}

void signalHandler(int signal)
{

	switch (signal)
	{
	case SIGUSR1:
		miLogInfo("Me llegó la señal %d, inicio compactación", signal);
		compactar();
		break;
	case SIGUSR2:
		miLogInfo("Me llegó la señal %d, inicio dump", signal);
		dump();
		break;
	case SIGINT:
		miLogInfo("Se forzó el cierre MiRam.");
		miLogDestroy();
		finalizar_memoria();
		exit(130); //Control+C
		break;
	default:
		break;
	}
}

// Lee la configuración y la deja disponible
int leer_config(void)
{
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if (config == NULL)
	{
		return EXIT_FAILURE;
	}

	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	configuracion->esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	configuracion->tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	configuracion->tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
	configuracion->path_swap = config_get_string_value(config, "PATH_SWAP");
	configuracion->algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	configuracion->criterio_seleccion = config_get_string_value(config, "CRITERIO_SELECCION");
	configuracion->mapa = config_get_string_value(config, "MAPA");

	return EXIT_SUCCESS;
}


static void char_destroy(char* self) {
    free(self);
}

// Atiende con hilos los request que son enviados a MiRAM
void atender_request_miram(Request *request)
{

	op_code codigo_operacion = request->codigo_operacion;
	t_buffer *buffer_devolucion;
	int request_fd = request->request_fd;
	t_list *lista;
	t_paquete *paquete_devuelto;
	t_paquete *paquete_devuelto_iniciar_patota;
	t_list *lista_mensajes;
	int resultado;

	switch (codigo_operacion)
	{

	case EXPULSAR_TRIPULANTE:
		//recibo los mensajes
		pthread_mutex_lock(&mutex_expulsion);
		t_buffer *buffer_devolucion_expulsar = request->buffer_devolucion;
		lista_mensajes = list_create();
		//recibo los mensajes
		lista = deserializar_lista_strings(buffer_devolucion_expulsar);

		int tripulante_expulsion = atoi(list_get(lista, 0));

		//miLogInfo("Me llego operacion: Expulsar tripulante %d", tripulante_expulsion);
		resultado = expulsar_tripulante(tripulante_expulsion);

		if (resultado == -1)
		{
			miLogInfo("ERROR: tripulante %d no expulsado", tripulante_expulsion);
			paquete_devuelto = crear_paquete(FAIL);
			list_add(lista_mensajes, "Se produjo un error al expulsar el tripulante");
		}
		else
		{
			miLogInfo("Tripulado %d expulsado correctamente", tripulante_expulsion);

			paquete_devuelto = crear_paquete(OK);
			list_add(lista_mensajes, string_itoa(tripulante_expulsion));
		}

		t_buffer *buffer_respuesta_expulsar = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer_respuesta_expulsar;
		enviar_paquete(paquete_devuelto, request_fd);
		eliminar_buffer(buffer_devolucion_expulsar);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		//eliminar_paquete(paquete_devuelto);
		free(request);
		pthread_mutex_unlock(&mutex_expulsion);
		dump_memoria(true);
		break;

	case INICIAR_TRIPULANTE:
		pthread_mutex_lock(&mutex_tripulantes);
		t_paquete *paquete_devuelto_iniciar_tripulante;
		t_buffer *buffer_devolucion_iniciar_tripulante = request->buffer_devolucion;
		lista_mensajes = list_create();
		TCB* un_tripulante;

		//recibo los mensajes
		lista = deserializar_lista_strings(buffer_devolucion_iniciar_tripulante);

		int PATOTA_ID = atoi(list_get(lista, 0));
		//miLogInfo("Me llego operacion: INICIAR TRIPULANTE PATOTA %d", PATOTA_ID);
		un_tripulante = iniciar_tripulante(PATOTA_ID);
		
		char *posicion = string_new();
		string_append(&posicion,string_itoa(un_tripulante->pos_X));
		string_append(&posicion,"|");
		string_append(&posicion,string_itoa(un_tripulante->pos_y));

		char *proxima_tarea;
		resultado = 1;
		if (resultado == -1)
		{
			miLogInfo("ERROR: Tripulante no iniciado");
			paquete_devuelto_iniciar_tripulante = crear_paquete(FAIL);
			list_add(lista_mensajes, "Se produjo un error al iniciar el tripulante");
		}
		else
		{
			miLogInfo("Tripulante %d iniciado correctamente", un_tripulante->TID);

			proxima_tarea = proxima_tarea_tripulante(un_tripulante->TID);

			paquete_devuelto_iniciar_tripulante = crear_paquete(OK);
			list_add(lista_mensajes, string_itoa(un_tripulante->TID));
			list_add(lista_mensajes, posicion);
			list_add(lista_mensajes, proxima_tarea);
		}

		t_buffer *buffer_respuesta_iniciar_tripulante = serializar_lista_strings(lista_mensajes);
		paquete_devuelto_iniciar_tripulante->buffer = buffer_respuesta_iniciar_tripulante;
		enviar_paquete(paquete_devuelto_iniciar_tripulante, request_fd);
		eliminar_buffer(buffer_devolucion_iniciar_tripulante);
		free(posicion);
		//eliminar_paquete(paquete_devuelto_iniciar_tripulante);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		free(proxima_tarea);
		free(request);
		pthread_mutex_unlock(&mutex_tripulantes);
		break;

	case COMPACTACION:
		//recibo los mensajes
		lista_mensajes = list_create();
		t_buffer *buffer_devolucion_compactar = request->buffer_devolucion;

		//recibo los mensajes
		miLogInfo("Me llego operacion: COMPACTAR");
		lista = deserializar_lista_strings(buffer_devolucion_compactar);
		resultado = compactar();

		if (resultado == -1)
		{
			miLogInfo("ERROR: COMPACTACION CON ERROR");
			paquete_devuelto = crear_paquete(FAIL);
			list_add(lista_mensajes, "Se produjo un error al compactar");
		}
		else
		{
			miLogInfo("COMPACTACION OK");

			paquete_devuelto = crear_paquete(OK);
			list_add(lista_mensajes, "OK");
		}

		t_buffer *buffer_respuesta_compactar = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer_respuesta_compactar;
		enviar_paquete(paquete_devuelto, request_fd);
		eliminar_buffer(buffer_devolucion_compactar);
		//eliminar_paquete(paquete_devuelto);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		free(request);

		break;

	case INICIAR_PATOTA:

		pthread_mutex_lock(&mutex_patota);
		t_buffer *buffer_devolucion_iniciar_patota = request->buffer_devolucion;
		//recibo los mensajes
		//miLogInfo("Me llego operacion: INICIAR PATOTA");

		lista = deserializar_lista_strings(buffer_devolucion_iniciar_patota);

		int cantidad_tripulantes = atoi(list_get(lista, 0));
		char *tareas = list_get(lista, 1);
		char *puntos = list_get(lista, 2);

		lista_mensajes = list_create();
		resultado = iniciar_patota(cantidad_tripulantes, tareas, puntos);

		if (resultado == -1)
		{
			miLogInfo("ERROR: patota no iniciada");

			paquete_devuelto_iniciar_patota = crear_paquete(FAIL);
			list_add(lista_mensajes, "Se produjo un error al iniciar la patota");
		}
		else
		{
			miLogInfo("Patota %d iniciada correctamente", resultado);

			paquete_devuelto_iniciar_patota = crear_paquete(OK);
			list_add(lista_mensajes, string_itoa(resultado));
		}

		t_buffer *buffer_respuesta_iniciar_patota = serializar_lista_strings(lista_mensajes);
		paquete_devuelto_iniciar_patota->buffer = buffer_respuesta_iniciar_patota;

		enviar_paquete(paquete_devuelto_iniciar_patota, request_fd);
		eliminar_buffer(buffer_devolucion_iniciar_patota);
		//eliminar_paquete(paquete_devuelto_iniciar_patota);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		free(request);
		pthread_mutex_unlock(&mutex_patota);
		dump_memoria(true);
		break;

	case TAREA_SIGUIENTE:
		pthread_mutex_lock(&mutex_tareas);
		t_buffer *buffer_devolucion_tareas = request->buffer_devolucion;

		lista_mensajes = list_create();

		//recibo los mensajes
		lista = deserializar_lista_strings(buffer_devolucion_tareas);

		int tripulante_id = atoi(list_get(lista, 0));
		//miLogInfo("Me llego operacion: SIGUIENTE TAREA de tripulante %d", tripulante_id);

		char *tarea = proxima_tarea_tripulante(tripulante_id);

		miLogInfo("Proxima tarea de tripulante %d enviada", tripulante_id, tarea);
		paquete_devuelto = crear_paquete(OK);
		list_add(lista_mensajes, tarea);

		t_buffer *buffer_respuesta_tareas = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer_respuesta_tareas;
		enviar_paquete(paquete_devuelto, request_fd);
		eliminar_buffer(buffer_devolucion_tareas);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		

		free(request);
		pthread_mutex_unlock(&mutex_tareas);
		break;

	case MOV_TRIPULANTE:

		pthread_mutex_lock(&mutex_mover);
		t_buffer *buffer_devolucion_mover = request->buffer_devolucion;

		lista_mensajes = list_create();

		//recibo los mensajes
		lista = deserializar_lista_strings(buffer_devolucion_mover);

		int tripulante_id_a_mover = atoi(list_get(lista, 0));
		//miLogInfo("Me llego operacion: MOVER de tripulante %d", tripulante_id_a_mover);

		char *eje = list_get(lista, 1);
		int nueva_posicion = atoi(list_get(lista, 2));

		if (strcmp(eje, "X") == 0)
		{
			mover_tripulante_en_x(tripulante_id_a_mover, nueva_posicion);
		}
		else
		{
			mover_tripulante_en_y(tripulante_id_a_mover, nueva_posicion);
		}

		miLogInfo("Tripulante %d se movio", tripulante_id_a_mover);
		paquete_devuelto = crear_paquete(OK);
		list_add(lista_mensajes, "OK");

		t_buffer *buffer_respuesta_mover = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer_respuesta_mover;
		enviar_paquete(paquete_devuelto, request_fd);
		eliminar_buffer(buffer_devolucion_mover);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		free(request);
		pthread_mutex_unlock(&mutex_mover);
		break;

case CAMBIO_COLA:

		pthread_mutex_lock(&mutex_mover);
		t_buffer *buffer_devolucion_cambio_cola = request->buffer_devolucion;

		lista_mensajes = list_create();

		//recibo los mensajes
		lista = deserializar_lista_strings(buffer_devolucion_cambio_cola);

		int tripulante_id_a_cambiar = atoi(list_get(lista, 0));
		//miLogInfo("Me llego operacion: MOVER de tripulante %d", tripulante_id_a_mover);

		char *cola_nueva = list_get(lista, 1);
		char cola_destino;

		if (strcmp(cola_nueva,"READY") == 0)
		{
			cola_destino = 'R';
		}
		if (strcmp(cola_nueva,"EXEC") == 0)
		{
			cola_destino = 'E';
		}
		if (strcmp(cola_nueva,"BLOCKED_IO") == 0)
		{
			cola_destino = 'B';
		}

		cambiar_cola_tripulante(tripulante_id_a_cambiar, cola_destino);

		miLogInfo("Tripulante %d paso a %s", tripulante_id_a_mover, cola_nueva);
		paquete_devuelto = crear_paquete(OK);
		list_add(lista_mensajes, "OK");

		t_buffer *buffer_respuesta_cambiar = serializar_lista_strings(lista_mensajes);
		paquete_devuelto->buffer = buffer_respuesta_cambiar;
		enviar_paquete(paquete_devuelto, request_fd);
		eliminar_buffer(buffer_devolucion_cambio_cola);
		list_destroy(lista_mensajes);		
		list_destroy(lista);		
		free(request);
		pthread_mutex_unlock(&mutex_mover);
		break;

	default:
		miLogInfo("Operación recibida no existe");

		break;
	}


}




// Crea la grilla inicial y la muestra en pantalla
void crear_grilla(void)
{
	int err;
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&cols, &rows);
	nivel = nivel_crear("AMONGOS");
	nivel_gui_dibujar(nivel);
}

void crear_personaje_grilla(int tripulante, int pos_x, int pos_y)
{
	char identificador = identificadores[proximo_identificador++];

	Identidad_grilla *id_grilla = malloc(sizeof(Identidad_grilla));
	id_grilla->TID = tripulante;
	id_grilla->identificador = identificador;

	list_add(tabla_identificadores_grilla, id_grilla);
	personaje_crear(nivel, identificador, pos_x, pos_y);
	nivel_gui_dibujar(nivel);
}

//Mueve un tripulante a una dirección destino
int mover_tripulante_en_x(int tripulante, int posicion_x_final)
{
	bool mapa = false;
	if (strcmp(configuracion->mapa, "HABILITADO") == 0)
	{
		mapa = true;
	}

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		mover_tripulante_en_x_segmentacion(tripulante, posicion_x_final, mapa);
	}
	else
	{
		mover_tripulante_en_x_paginacion(tripulante, posicion_x_final, mapa);
	}
}

int cambiar_cola_tripulante(int tripulante, char cola_destino)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		cambiar_cola_tripulante_segmentacion(tripulante, cola_destino);
	}
	else
	{
		cambiar_cola_tripulante_paginacion(tripulante, cola_destino);
	}
}

int mover_tripulante_en_y(int tripulante, int posicion_y_final)
{
	bool mapa = false;
	if (strcmp(configuracion->mapa, "HABILITADO") == 0)
	{
		mapa = true;
	}

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		mover_tripulante_en_y_segmentacion(tripulante, posicion_y_final, mapa);
	}
	else
	{
		mover_tripulante_en_y_paginacion(tripulante, posicion_y_final, mapa);
	}
}

void mover_tripulante_grilla(char identificador, int pos_x, int pos_y)
{

	int err;

	err = item_desplazar(nivel, identificador, pos_x, pos_y);
	nivel_gui_dibujar(nivel);
}

void expulsar_tripulante_grilla(char identificador)
{

	int err;

	err = item_borrar(nivel, identificador);
	nivel_gui_dibujar(nivel);
}

//buscar el caracter asignado en la grilla, para el tripulante
char buscar_tripulante_grilla(int tripulante)
{
	bool encontre_tripulante = false;
	char identificador_tripulante;
	TCB *unTCB;
	TCB *TCB_encontrado;

	t_list_iterator *list_iterator = list_iterator_create(tabla_identificadores_grilla);
	Identidad_grilla *id_grilla;

	while (list_iterator_has_next(list_iterator))
	{
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
	}
	else
	{
		miLogInfo("El tripulante %d a mover no existe", tripulante);
		return '-';
	}
}

void inicializar_memoria(int tamanio_memoria)
{
	MEMORIA = malloc(tamanio_memoria);

	miLogInfo("Inicia MEMORIA, primera direccion %p", MEMORIA);

	contador_patotas = 0;
	contador_tripulantes = 0;
	tabla_identificadores_grilla = list_create();

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		inicializar_segmentacion(tamanio_memoria, configuracion->criterio_seleccion);
	}
	else
	{
		char* algoritmo_reemplazo = configuracion->algoritmo_reemplazo;
		tamanio_pagina = configuracion->tamanio_pagina;
		inicializar_paginacion(tamanio_memoria, tamanio_pagina, configuracion->tamanio_swap, configuracion->path_swap, algoritmo_reemplazo);
	}
}

void finalizar_memoria()
{
	dump_memoria(true);

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		finalizar_segmentacion();
	}
	else
	{
		finalizar_paginacion(tamanio_memoria);
	}

	free(MEMORIA);

	//nivel_destruir(nivel);
	//nivel_gui_terminar();
}

int compactar()
{

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return compactar_segmentacion();
	}
	else
	{
		return -1;
	}
}

int dump()
{

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return archivo_segmentacion(true);
	}
	else
	{
		return archivo_paginacion(true);
	}
}

void dump_memoria(bool mostrar_vacios)
{
	if (strcmp(configuracion->mapa, "HABILITADO") != 0)
	{
		if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
		{
			dump_memoria_segmentacion(mostrar_vacios);
		}
		else
		{
			dump_memoria_paginacion(mostrar_vacios);
		}
	}
}

char *obtener_punto_string(char *puntos, int i)
{
	char **lista_puntos;
	lista_puntos = string_split(puntos, ";");
	char *string_punto = strdup(lista_puntos[i]);

	string_iterate_lines(lista_puntos, (void *)free);
	free(lista_puntos);
	return string_punto;
}

int reservar_memoria(int bytes)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		if (strcmp(configuracion->criterio_seleccion, "FF") == 0)
		{
			return reservar_memoria_segmentacion_ff(bytes);
		}
		else
		{
			return reservar_memoria_segmentacion_bf(bytes);
		}
	}
	else
	{
		return reservar_memoria_paginacion(bytes);
	}
}

int alta_patota(PCB *unPCB)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return alta_patota_segmentacion(unPCB);
	}
	else
	{
		return alta_patota_paginacion(unPCB);
	}
}

u_int32_t buscar_patota(int PCB_ID)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return buscar_patota_segmentacion(PCB_ID);
	}
	else
	{
		return buscar_patota_paginacion(PCB_ID);
	}
}

void alta_tareas(int PCB_ID, char *tareas)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return alta_tareas_segmentacion(PCB_ID, tareas);
	}
	else
	{
		return alta_tareas_paginacion(PCB_ID, tareas);
	}
}

u_int32_t buscar_tripulante(int TCB_ID)
{
	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return buscar_tripulante_segmentacion(TCB_ID);
	}
	else
	{
		return buscar_tripulante_paginacion(TCB_ID);
	}
}

u_int32_t iniciar_tareas(int PCB_ID, char *tareas)
{

	u_int32_t posicion_memoria = reservar_memoria(strlen(tareas));

	if (posicion_memoria == 99)
	{
		return 99;
	}

	memcpy(posicion_memoria, tareas, strlen(tareas));

	alta_tareas(PCB_ID, posicion_memoria);

	return posicion_memoria;
}

char *proxima_tarea_tripulante(int tripulante_id)
{

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		proxima_tarea_tripulante_segmentacion(tripulante_id);
	}
	else
	{
		proxima_tarea_tripulante_paginacion(tripulante_id);
	}
}

char *buscar_posicion_tripulante(int tripulante_id)
{

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		buscar_posicion_tripulante_segmentacion(tripulante_id);
	}
	else
	{
		buscar_posicion_tripulante_paginacion(tripulante_id);
	}
}

int iniciar_patota(int cantidad_tripulantes, char *tareas, char *puntos)
{

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		iniciar_patota_segmentacion(cantidad_tripulantes, tareas, puntos);
	}
	else
	{
		iniciar_patota_paginacion(cantidad_tripulantes, tareas, puntos);
	}
}

TCB* iniciar_tripulante(int patota_id)
{
	bool mapa = false;
	if (strcmp(configuracion->mapa, "HABILITADO") == 0)
	{
		mapa = true;
	}

	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{
		return iniciar_tripulante_segmentacion(patota_id, mapa);
	}
	else
	{
		return iniciar_tripulante_paginacion(patota_id, mapa);
	}
}

int expulsar_tripulante(int tripulante_id)
{
	int resultado;
	bool mapa = false;

	if (strcmp(configuracion->mapa, "HABILITADO") == 0)
	{
		mapa= true;
	}



	if (strcmp(configuracion->esquema_memoria, "SEGMENTACION") == 0)
	{

		u_int32_t posicion_memoria = buscar_tripulante(tripulante_id);

		if (posicion_memoria == 99)
		{
			return -1;
		}
		else
		{
			resultado = expulsar_tripulante_segmentacion(tripulante_id, mapa);
		}
	} else {
		resultado = expulsar_tripulante_paginacion(tripulante_id, mapa);
	}
}

void *iniciar_servidor_miram()
{
	levantar_servidor(atender_request_miram, puerto_miram);
	return NULL;
}

void *iniciar_funciones_memoria()
{
	inicializar_memoria(tamanio_memoria);
	dump_memoria(true);
	return NULL;
}

int main()
{
	//Signal para atender la compactación
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, false, LOG_LEVEL_INFO);
	miLogInfo("Inició MiRAM.");

	if (leer_config())
	{
		miLogInfo("Error al iniciar MiRAM: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	tamanio_memoria = configuracion->tamanio_memoria;

	puerto_miram = string_itoa(configuracion->puerto);

	iniciar_funciones_memoria();

	if (pthread_create(&threadSERVER, NULL, (void *)iniciar_servidor_miram,
					   NULL) != 0)
	{
		printf("Error iniciando servidor/n");
	}

	if (strcmp(configuracion->mapa, "HABILITADO") == 0)
	{
		if (pthread_create(&threadMAPA, NULL, (void *)crear_grilla,
						   NULL) != 0)
		{
			printf("Error creando grilla/n");
		}
	}
	pthread_join(threadSERVER, NULL);
	pthread_join(threadMAPA, NULL);

	miLogInfo("Finalizó MiRAM");
	free(configuracion);
	finalizar_memoria();
	miLogDestroy();
}
