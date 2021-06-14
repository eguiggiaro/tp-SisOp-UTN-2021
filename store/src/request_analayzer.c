#include "request_analyzer.h"
#include "ejecucion_tareas.h"
#include "store.h"

const char *tipoTarea_table [] = { 
	"GENERAR_OXIGENO",
    "GENERAR_COMIDA",
    "GENERAR_BASURA",
    "CONSUMIR_OXIGENO",
    "CONSUMIR_COMIDA",
    "DESECHAR_BASURA", 
	NULL };

void atender_request_store(Request *request) {

	op_code codigo_operacion = request->codigo_operacion;
	t_buffer *buffer_devolucion;
	int request_fd = request->request_fd;
	t_list *lista;
	t_paquete *paquete_devuelto;
	t_list *lista_mensajes;
	int resultadoTarea;
	int resultadoBitacora;

	switch(codigo_operacion)
	{	

		case INFORMAR_TAREA: //por ejemplo: GENERAR_OXIGENO 12

			pthread_mutex_lock(&mutex_informartareas);
			t_buffer* buffer_devolucion_informar_tarea = request->buffer_devolucion;
			
			miLogInfo("Me llego operacion: INFORMAR_TAREA \n");
			lista = deserializar_lista_strings(buffer_devolucion);

			char* id_tripulante = list_get(lista,0); //Ej: id_tripulante.  
			char* informeTarea = list_get(lista,1);  //Ej: GENERAR_OXIGENO 12
			char** lista;

			lista = string_split(informeTarea[1] , " ");//Crea una lista separando la cadena informeTarea[1] por SPACE. Resultado lista[0]= GENERAR_OXIGENO . lista[1] = 12
	
			char* tarea = list_get(lista,0);//tarea
			int cantidadRecursos = atoi(list_get(lista,1)); //En el caso de DESCARTAR_BASURA es NULL

	 		resultadoTarea = ejecutarTarea(tarea, cantidadRecursos);
			// resultadoBitacora = guardarEnBitacora(tarea);

			if (resultadoTarea == -1)
			{
				miLogInfo("ERROR: NO SE PUDO REALIZAR LA TAREA \n");
				paquete_devuelto = crear_paquete(FAIL);
				list_add(lista_mensajes, "Se produjo un error intentar realizar la tarea");
			}
			else
			{
				miLogInfo("TAREA REALIZADA CORRECTAMENTE \n");

				paquete_devuelto = crear_paquete(OK);
				list_add(lista_mensajes, "Se realizó la tarea correctamente");
			}

			t_buffer *buffer_respuesta_informarTarea = serializar_lista_strings(lista_mensajes);
			paquete_devuelto->buffer = buffer_respuesta_informarTarea;
			enviar_paquete(paquete_devuelto, request_fd);
			eliminar_buffer(buffer_devolucion_informar_tarea);
			list_destroy(lista_mensajes);
			list_destroy(lista);
			free(request);
			pthread_mutex_unlock(&mutex_informartareas);

	  	break;
		
		default:
			miLogInfo("Me llego operacion: ...\n");
	  	break;
	}
}


int ejecutarTarea(char* tarea, int cantidadRecursos){

		tipoTarea opc = find_enum_consola(tarea);

		switch (opc)
		{
				case GENERAR_OXIGENO:
					printf( "Tarea es GENERAR_OXIGENO\n" );
					generarRecursos(OXIGENO, cantidadRecursos);
					break;

				case CONSUMIR_OXIGENO:
					printf( "Tarea es CONSUMIR_OXIGENO\n" );
					break;

				case GENERAR_COMIDA:
					printf( "Tarea es GENERAR_COMIDA\n" );
					generarRecursos(COMIDA, cantidadRecursos);
					break;

				case CONSUMIR_COMIDA:
					printf( "Tarea es CONSUMIR_COMIDA\n" );
					break;

				case GENERAR_BASURA:
					printf( "Tarea es GENERAR_BASURA\n" );
					generarRecursos(BASURA, cantidadRecursos);
					break;

				case DESECHAR_BASURA:
					printf( "Tarea es DESECHAR_BASURA\n" );
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