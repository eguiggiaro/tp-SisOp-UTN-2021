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

void atender_request_store(uint32_t request_fd) {

	op_code codigo_operacion = recibir_operacion(request_fd);
	t_buffer* buffer_devolucion;

	switch(codigo_operacion)
	{	
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
      list_add(lista_mensajes, "soy store");

	 	 	buffer_devolucion = serializar_lista_strings(lista_mensajes);
	  	paquete_devuelto->buffer = buffer_devolucion;
	  	enviar_paquete(paquete_devuelto, request_fd);

	  	break;
	
	  case MENSAJE:
			miLogInfo("Me llego operacion: MENSAJE\n");
	  	break;

	  case INFORMAR_TAREA:

			miLogInfo("Me llego operacion: INICIAR PATOTA \n");
			buffer_devolucion = recibir_buffer(request_fd);

			lista = deserializar_lista_strings(buffer_devolucion);

			char* id_tripulante = list_get(lista,0); //Ej: id_tripulante.  
			char* informeTarea = list_get(lista,1);  //Ej: GENERAR_OXIGENO 12;2;3
			char** lista1;
			char** lista2;

			lista1 = string_split(informeTarea[1], ";");//Crea una lista separando la cadena informeTarea[1] por ;. Resultado: lista1[0]= GENERAR_OXIGENO 12. ls[1]=2. ls [2]=3
			lista2 = string_split(lista1[0], " ");//Crea una lista separando la cadena lista[0] por SPACE. Resultado lista2[0]= GENERAR_OXIGENO . ls2[1] = 12
	
			char* tarea = list_get(lista2,0);//tarea
			int cantidadRecursos = atoi(list_get(lista2,1)); //En el caso de DESCARTAR_BASURA es NULL
			int posX = atoi(list_get(lista1,1));
			int posY = atoi(list_get(lista1,2));

	 		ejecutarTarea(tarea, cantidadRecursos);

			miLogInfo("Me llego operacion: INFORMAR_TAREA\n");

	  	break;

	  default:
			miLogInfo("Me llego operacion: ...\n");
	  	break;
	}
}


void ejecutarTarea(char* tarea, int cantidadRecursos){

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
}

tipoTarea find_enum_consola(char *sval)
{
	tipoTarea result=GENERAR_OXIGENO; /* value corresponding to etable[0] */
  int i=0;
  for (i=0; tipoTarea_table[i]!=NULL; ++i, ++result)
    if (0==strcmp(sval, tipoTarea_table[i])) return result;
  return -1;
}