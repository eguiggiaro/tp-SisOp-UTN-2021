#include "request_analyzer.h"
#include "ejecucion_tareas.h"
#include "store.h"

const char *tipoTarea_table [] = { 
	"GENERAR_OXIGENO",
    "GENERAR_COMIDA",
    "GENERAR_BASURA",
    "CONSUMIR_OXIGENO",
    "CONSUMIR_COMIDA",
    "DESCARTAR_BASURA", 
	NULL };

void atender_request_store(Request *request) {

	op_code codigo_operacion = request->codigo_operacion;
	int request_fd = request->request_fd;
	t_list *lista;
	t_list *lista_mensajes;
	int resultadoTarea;
	int resultadoBitacora;
	char* id_tripulante; 
	char* mensaje;

	switch(codigo_operacion)
	{	

		case INFORMAR_TAREA: //por ejemplo: GENERAR_OXIGENO 12

			pthread_mutex_lock(&mutex_informartareas);
			t_buffer* buffer_devolucion_informar_tarea = request->buffer_devolucion;
			t_paquete* paquete_devuelto_informar_tarea;
			lista_mensajes = list_create();

			miLogDebug("Me llego operacion: INFORMAR_TAREA.");
			lista = deserializar_lista_strings(buffer_devolucion_informar_tarea);

			id_tripulante = list_get(lista,0); //Ej: id_tripulante. 
			 
			char* informeTarea = list_get(lista,1);  //Ej: GENERAR_OXIGENO 12
			miLogInfo("Me llego la tarea: %s, y corresponde al tripulante: %s.", informeTarea, id_tripulante);

			char** listaNueva;
			listaNueva = string_split(informeTarea , " ");//Crea una lista separando la cadena informeTarea[1] por SPACE. Resultado lista[0]= GENERAR_OXIGENO . lista[1] = 12
	
			char* tarea = listaNueva[0];//tarea
			int cantidadRecursos = atoi(listaNueva[1]); //En el caso de DESCARTAR_BASURA es NULL
		
	 		resultadoTarea = ejecutarTarea(tarea, cantidadRecursos);
			char* mensajeAGuardar = string_new();
			string_append(&mensajeAGuardar, "Comienza ejecucion tarea ");
			string_append(&mensajeAGuardar, informeTarea);
			string_append(&mensajeAGuardar, "\n");
			resultadoBitacora = guardarEnBitacora(id_tripulante, mensajeAGuardar);	

			if (resultadoTarea == -1)
			{
				miLogInfo("ERROR: No se pudo realizar la tarea: %s del tripulante: %s.", informeTarea, id_tripulante);
				paquete_devuelto_informar_tarea = crear_paquete(FAIL);
				mensaje = strdup("Se produjo un error intentar realizar la tarea");
				list_add(lista_mensajes, mensaje);
			}
			else
			{
				miLogInfo("La tarea: %s del tripulante: %s, se realizo correctamente.", informeTarea, id_tripulante);

				paquete_devuelto_informar_tarea = crear_paquete(OK);
				mensaje = strdup("Se realizo la tarea correctamente");
				list_add(lista_mensajes, mensaje);
			}

			t_buffer *buffer_respuesta_informarTarea = serializar_lista_strings(lista_mensajes);
			paquete_devuelto_informar_tarea->buffer = buffer_respuesta_informarTarea;
			enviar_paquete(paquete_devuelto_informar_tarea, request_fd);
			eliminar_buffer(buffer_devolucion_informar_tarea);

			list_destroy_and_destroy_elements(lista_mensajes, (void*) char_destroy);
			list_destroy_and_destroy_elements(lista, (void*) char_destroy);
			liberar_array(listaNueva);
			
			free(mensajeAGuardar);	
			free(request);

			pthread_mutex_unlock(&mutex_informartareas);

	  	break;

		case INFORMACION_BITACORA:

			pthread_mutex_lock(&mutex_informacionBitacora);
			t_buffer* buffer_devolucion_informacion_bitacora = request->buffer_devolucion;
			t_paquete *paquete_devuelto_informacion_bitacora;
			lista_mensajes = list_create();

			miLogDebug("Me llego operacion: INFORMACION_BITACORA");
			lista = deserializar_lista_strings(buffer_devolucion_informacion_bitacora);

			id_tripulante = list_get(lista,0); //Ej: id_tripulante.  
			char* instruccionABitacora;// = string_new();  //Ej: Se finaliza tarea X						
			char* instruccionALog;// = string_new();
			
			instruccionABitacora = strdup(list_get(lista,1));  //Ej: Se finaliza tarea X						
			instruccionALog = strdup(instruccionABitacora);
			miLogInfo("Me llego la tarea: %s, y corresponde al tripulante: %s.", instruccionALog, id_tripulante);

			string_append(&instruccionABitacora, "\n");
			resultadoTarea = guardarEnBitacora(id_tripulante, instruccionABitacora);

			if (resultadoTarea == -1)
			{
				miLogInfo("ERROR: No se pudo guardar la tarea: %s, en la bitacora del tripulante: %s.", instruccionALog, id_tripulante);
				paquete_devuelto_informacion_bitacora = crear_paquete(FAIL);
				mensaje = strdup("Se produjo un error intentar guardar la instruccion");
				list_add(lista_mensajes, mensaje);
			}
			else
			{
				miLogInfo("Se guardo la tarea: %s, en la bitacora del tripulante: %s.", instruccionALog, id_tripulante);

				paquete_devuelto_informacion_bitacora = crear_paquete(OK);
				mensaje = strdup("Se guardo la tarea correctamente");
				list_add(lista_mensajes, mensaje);
			}
			t_buffer *buffer_respuesta_informacion_bitacora = serializar_lista_strings(lista_mensajes);
			paquete_devuelto_informacion_bitacora->buffer = buffer_respuesta_informacion_bitacora;
			enviar_paquete(paquete_devuelto_informacion_bitacora, request_fd);
			eliminar_buffer(buffer_devolucion_informacion_bitacora);
			
			list_destroy_and_destroy_elements(lista_mensajes, (void*) char_destroy);
			list_destroy_and_destroy_elements(lista, (void*) char_destroy);
			free(instruccionABitacora);
			free(instruccionALog);
			free(request);
			pthread_mutex_unlock(&mutex_informacionBitacora);

		break;
		
		case OBTENER_BITACORA:

			pthread_mutex_lock(&mutex_obtenerBitacora);
			t_buffer* buffer_devolucion_obtener_bitacora = request->buffer_devolucion;
			t_paquete *paquete_devuelto_obtener_bitacora;
			lista_mensajes = list_create();

			miLogDebug("Me llego operacion: OBTENER BITACORA");
			lista = deserializar_lista_strings(buffer_devolucion_obtener_bitacora);

			id_tripulante = list_get(lista,0); //Ej: id_tripulante.  
			miLogInfo("Me solicitaron la bitacora del tripulante: %s.", id_tripulante);
			
			char* bitacora = obtenerBitacora(id_tripulante);

			if (string_is_empty(bitacora))
			{
				miLogDebug("ERROR: LA BITACORA DEL TRIPULANTE ESTA VACIA.");
				paquete_devuelto_obtener_bitacora = crear_paquete(FAIL);
				char* mensajeError = strdup("La bitacora del tripulante esta vacia");
				list_add(lista_mensajes, mensajeError);
			}
			else
			{
				miLogInfo("Se envio la bitacora del tripulante: %s.", id_tripulante);

				paquete_devuelto_obtener_bitacora = crear_paquete(OK);
				list_add(lista_mensajes, bitacora);
			}
			t_buffer* buffer_respuesta_obtener_bitacora = serializar_lista_strings(lista_mensajes);
			paquete_devuelto_obtener_bitacora->buffer = buffer_respuesta_obtener_bitacora;
			enviar_paquete(paquete_devuelto_obtener_bitacora, request_fd);
			eliminar_buffer(buffer_devolucion_obtener_bitacora);
			
			list_destroy_and_destroy_elements(lista_mensajes, (void*) char_destroy);
			list_destroy_and_destroy_elements(lista, (void*) char_destroy);
			free(request);

			pthread_mutex_unlock(&mutex_obtenerBitacora);

		break;
		
		case FSCK:
			//Activar el sabotaje	
			miLogDebug("Me llego operacion: FSCK");
			t_buffer* buffer_devolucion_fsck = request->buffer_devolucion;
			t_paquete *paquete_devuelto_fsck;
			lista_mensajes = list_create();

			lista = deserializar_lista_strings(buffer_devolucion_fsck);

			id_tripulante = list_get(lista,0); //Ej: id_tripulante.  
			char* posX = list_get(lista,1);
			char* posY = list_get(lista,2);
			miLogInfo("El tripulante: %s, va a solucionar el sabotaje en la posicion: %s|%s.", id_tripulante, posX, posY);
			
			pthread_mutex_lock(&mutexEjecucionSabotaje);
			esperaSabotaje = 1;
			int resultado = pthread_cond_signal(&condEjecucionSabotaje);
			
			if (resultado != 0)
			{
				miLogError("ERROR: NO SE PUDO ACTIVAR EL PROTOCOLO DE SABOTAJE FSCK.");
				paquete_devuelto_fsck = crear_paquete(FAIL);
				char* mensajeError = strdup("No se pudo activar el protocolo.");
				list_add(lista_mensajes, mensajeError);
			}
			else
			{
				miLogDebug("El protocolo FSCK fue activado correctamente y esta en ejecucion.");
				paquete_devuelto_fsck = crear_paquete(OK);

			}
			t_buffer* buffer_respuesta_fsck = serializar_lista_strings(lista_mensajes);
			paquete_devuelto_fsck->buffer = buffer_respuesta_fsck;
			enviar_paquete(paquete_devuelto_fsck, request_fd);
			eliminar_buffer(buffer_devolucion_fsck);
		
			list_destroy_and_destroy_elements(lista_mensajes, (void*) char_destroy);
			list_destroy_and_destroy_elements(lista, (void*) char_destroy);
			free(request);
			pthread_mutex_unlock(&mutexEjecucionSabotaje);
			break;		

		default:
			miLogInfo("Me llego operacion: ...");
	  	break;
	}	
}


int ejecutarTarea(char* tarea, int cantidadRecursos){

		tipoTarea opc = find_enum_consola(tarea);
		int resultado;

		switch (opc)
		{
				case GENERAR_OXIGENO:
					//printf( "Tarea es GENERAR_OXIGENO\n" );
					resultado = generarRecursos(OXIGENO, cantidadRecursos);
					break;

				case CONSUMIR_OXIGENO:
					//printf( "Tarea es CONSUMIR_OXIGENO\n" );
					resultado = consumirRecursos(OXIGENO, cantidadRecursos);
					break;

				case GENERAR_COMIDA:
					//printf( "Tarea es GENERAR_COMIDA\n" );
					resultado = generarRecursos(COMIDA, cantidadRecursos);
					break;

				case CONSUMIR_COMIDA:
					//printf( "Tarea es CONSUMIR_COMIDA\n" );
					resultado = consumirRecursos(COMIDA, cantidadRecursos);
					break;

				case GENERAR_BASURA:
					//printf( "Tarea es GENERAR_BASURA\n" );
					resultado = generarRecursos(BASURA, cantidadRecursos);
					break;

				case DESCARTAR_BASURA:
					//printf( "Tarea es DESCARTAR_BASURA\n" );
					resultado = desecharRecurso(BASURA);
					break;

				default:
					printf( "Error: tarea inexistente\n" );
					break;
		}
		//aca tengo que devolver -1 si falló
		return 1;
}

tipoTarea find_enum_consola(char *sval)
{
	tipoTarea result=GENERAR_OXIGENO; /* value corresponding to etable[0] */
	int i=0;
	for (i=0; tipoTarea_table[i]!=NULL; ++i, ++result)
	if (0==strcmp(sval, tipoTarea_table[i])) return result;
	return -1;
}
