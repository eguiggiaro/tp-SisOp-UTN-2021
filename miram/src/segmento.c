#include "segmento.h"

void dump_memoria_segmentos(bool mostrar_vacios)
{


	printf("--------------------------------------------------------------------\n");
	printf("Detalle tabla de segmentos\n");
	printf("--------------------------------------------------------------------\n");
	printf("Dir Inicio\tId segmento\tDesplazamiento\tEstado\n");
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	while (list_iterator_has_next(list_iterator))
	{
		Segmento *segmento_aux = list_iterator_next(list_iterator);

		//Imprime solo segmentos libres o no, según parámetro de entrada
		if (mostrar_vacios || (segmento_aux->id) >= 0)
			imprimir_segmento(segmento_aux);
	}

	printf("--------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator);
}

void dump_memoria_contenido_segmentacion()
{

	printf("Detalle memoria\n");
	printf("--------------------------------------------------------------------\n");
	printf("Patota Id\t# Segmento\t  Tipo\t   Inicio\tTamaño\n");

	t_list_iterator *list_iterator_pcb = list_iterator_create(tabla_segmentos_pcb);
	t_list_iterator *list_iterator_tareas;
	t_list_iterator *list_iterator_tcb;


	PCB_adm* pcbadm;
	Tarea_adm* tareadm;
	TCB_adm* tcbadm;
	int patota, segmento, tipo, inicio, tamanio;
	Segmento* un_segmento;
	PCB* pcb;
	TCB* tcb;

	char* tareas;

	while (list_iterator_has_next(list_iterator_pcb))
	{
		pcbadm = list_iterator_next(list_iterator_pcb);
		patota = pcbadm->PID;
		segmento = pcbadm->segmento_nro;
		tipo = "PCB";
		un_segmento = buscar_segmento_por_id(segmento);
		inicio = un_segmento->dir_inicio;
		tamanio = un_segmento->desplazamiento;
		pcb = buscar_patota_segmentacion(pcbadm->PID);

		printf("%9d\t%10d\t%6s\t%p\t%5db\n", patota, segmento, tipo,pcb,tamanio);

		list_iterator_tareas = list_iterator_create(tabla_segmentos_tareas);

		while (list_iterator_has_next(list_iterator_tareas))
		{
			tareadm = list_iterator_next(list_iterator_tareas);

			if (tareadm->PID == pcbadm->PID)
			{
				segmento = tareadm->segmento_nro;
				tipo = "Tareas";
				un_segmento = buscar_segmento_por_id(segmento);
				inicio = un_segmento->dir_inicio;
				tamanio = un_segmento->desplazamiento;
				tareas = pcb->Tareas;

				printf("%9d\t%10d\t%4s\t%p\t%5db\n", patota, segmento, tipo,tareas,tamanio);
				break;
			}
		}
		list_iterator_destroy(list_iterator_tareas);

		list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);

		while (list_iterator_has_next(list_iterator_tcb))
			{
				tcbadm = list_iterator_next(list_iterator_tcb);

					if (tcbadm->PID == pcbadm->PID)
					{
						segmento = tcbadm->segmento_nro;
						tipo = "  TCB";
						un_segmento = buscar_segmento_por_id(segmento);
						inicio = un_segmento->dir_inicio;
						tamanio = un_segmento->desplazamiento;
						tcb = buscar_tripulante_segmentacion(tcbadm->TID);

						printf("%9d\t%10d\t%4s\t%p\t%5db\n", patota, segmento, tipo,tcb,tamanio);
					}
			}
		list_iterator_destroy(list_iterator_tcb);

	}

	printf("--------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator_pcb);
}



//Muestra el contenido de la tabla de segmentos
void dump_memoria_segmentacion(bool mostrar_vacios)

{
	pthread_mutex_lock(&mutex_dump);
	dump_memoria_segmentos(mostrar_vacios);
	dump_memoria_contenido_segmentacion();
	fflush(stdout); 
	pthread_mutex_unlock(&mutex_dump);

}

//Imprime el segmento
//TO DO no es muy amigable el resultado MEJORAR!

void imprimir_segmento(Segmento *segmento_a_imprimir)

{
	printf("%d\t%d\t\t%d\t\t%s\n", segmento_a_imprimir->dir_inicio, segmento_a_imprimir->id, segmento_a_imprimir->desplazamiento, segmento_a_imprimir->estado);
}

//Imprime el segmento, no es muy amigable, MEJORAR!
u_int32_t reservar_memoria_segmentacion_ff(int bytes)
{
	int index = 0;
	bool encontre_segmento = false;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	Segmento *segmento_a_ocupar;
	Segmento *segmento_nuevo;
	u_int32_t posicion_reservada;

	while (list_iterator_has_next(list_iterator))
	{
		segmento_a_ocupar = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if ((strcmp(segmento_a_ocupar->estado, "LIBRE") == 0) && (segmento_a_ocupar->desplazamiento >= bytes))
		{
			encontre_segmento = true;

			if (segmento_a_ocupar->desplazamiento == bytes)

			{
				segmento_a_ocupar->id = contador_segmentos++;
				segmento_a_ocupar->desplazamiento = bytes;
				segmento_a_ocupar->estado = "OCUPADO";
				posicion_reservada = segmento_a_ocupar->dir_inicio;
			}
			else
			{

				//Seteo el nuevo segmento
				segmento_nuevo = malloc(sizeof(Segmento));
				segmento_nuevo->id = contador_segmentos++;
				segmento_nuevo->dir_inicio = segmento_a_ocupar->dir_inicio;
				segmento_nuevo->desplazamiento = bytes;
				segmento_nuevo->estado = "OCUPADO";

				//Reduzco el segmento libre
				segmento_a_ocupar->dir_inicio = (segmento_nuevo->dir_inicio + (segmento_nuevo->desplazamiento));
				segmento_a_ocupar->desplazamiento = segmento_a_ocupar->desplazamiento - bytes;

				//Agrego el nuevo segmento a la tabla
				list_add_in_index(tabla_segmentos, index, segmento_nuevo);
				posicion_reservada = segmento_nuevo->dir_inicio;
				break;
			}
		}

		index++;
	}
	list_iterator_destroy(list_iterator);

	if (encontre_segmento)
	{
		return posicion_reservada;
	}
	else
	{
		return 99; //ERROR
	}
}

int buscar_segmento(u_int32_t posicion_memoria)
{
	bool encontre_segmento = false;
	int segmento_encontrado;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	Segmento *segmento_busqueda;

	while (list_iterator_has_next(list_iterator))
	{
		segmento_busqueda = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if ((segmento_busqueda->dir_inicio == posicion_memoria))
		{
			encontre_segmento = true;
			segmento_encontrado = segmento_busqueda->id;
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	if (encontre_segmento)
	{
		return segmento_encontrado;
	}
	else
	{
		return 99;
	}
}

Segmento* buscar_segmento_por_id(int id_segmento)
{
	bool encontre_segmento = false;
	Segmento* segmento_encontrado;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	Segmento *segmento_busqueda;

	while (list_iterator_has_next(list_iterator))
	{
		segmento_busqueda = list_iterator_next(list_iterator);

		if ((segmento_busqueda->id == id_segmento))
		{
			encontre_segmento = true;
			segmento_encontrado = segmento_busqueda;
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	if (encontre_segmento)
	{
		return segmento_encontrado;
	}
	else
	{
		return 99;
	}
}

u_int32_t buscar_posicion(int segmento)
{
	bool encontre_posicion = false;
	u_int32_t posicion_encontrada;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	Segmento *segmento_busqueda;

	while (list_iterator_has_next(list_iterator))
	{
		segmento_busqueda = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if ((segmento_busqueda->id == segmento))
		{
			encontre_posicion = true;
			posicion_encontrada = segmento_busqueda->dir_inicio;
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	if (encontre_posicion)
	{
		return posicion_encontrada;
	}
	else
	{
		return 99;
	}
}

void alta_patota_segmentacion(PCB *unPCB)
{
	PCB_adm *pcb_adm = malloc(sizeof(PCB_adm));
	pcb_adm->PID = unPCB->PID;
	pcb_adm->segmento_nro = buscar_segmento(unPCB);
	list_add(tabla_segmentos_pcb, pcb_adm);
}

uint32_t buscar_patota_segmentacion(int PID)
{
	bool encontre_patota = false;
	u_int32_t posicion_patota;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos_pcb);
	PCB_adm *pcb_adm;

	while (list_iterator_has_next(list_iterator))
	{
		pcb_adm = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if (pcb_adm->PID == PID)
		{
			encontre_patota = true;
			posicion_patota = buscar_posicion(pcb_adm->segmento_nro);
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	if (encontre_patota)
	{
		return posicion_patota;
	}
	else
	{
		return 99;
	}
}

void alta_tripulante_segmentacion(TCB *unTCB, int patota)
{
	TCB_adm *tcb_adm = malloc(sizeof(TCB_adm));
	tcb_adm->TID = unTCB->TID;
	tcb_adm->PID = patota;
	tcb_adm->segmento_nro = buscar_segmento(unTCB);
	list_add(tabla_segmentos_tcb, tcb_adm);
}

void alta_tareas_segmentacion(int PCB_ID, char *tareas)
{
	Tarea_adm *tareas_adm = malloc(sizeof(Tarea_adm));
	tareas_adm->PID = PCB_ID;
	tareas_adm->segmento_nro = buscar_segmento(tareas);
	list_add(tabla_segmentos_tareas, tareas_adm);
}

uint32_t buscar_tripulante_segmentacion(int TCB_ID)
{
	bool encontre_tripulante = false;
	u_int32_t posicion_tripulante;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos_tcb);
	TCB_adm *tcb_adm;

	while (list_iterator_has_next(list_iterator))
	{
		tcb_adm = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if (tcb_adm->TID == TCB_ID)
		{
			encontre_tripulante = true;
			posicion_tripulante = buscar_posicion(tcb_adm->segmento_nro);
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	if (encontre_tripulante)
	{
		return posicion_tripulante;
	}
	else
	{
		return 99;
	}
}

int expulsar_tripulante_segmentacion(int tripulante_id)
{
	Segmento *segmento;
	bool elimine_segmento = false;
	bool elimine_tripulante = false;
	int index = 0;

	t_list_iterator *list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);
	t_list_iterator *list_iterator_segmentos = list_iterator_create(tabla_segmentos);
	TCB_adm *tcb_adm;

	while (list_iterator_has_next(list_iterator_tcb))
	{
		tcb_adm = list_iterator_next(list_iterator_tcb);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if (tcb_adm->TID == tripulante_id)
		{

			while (list_iterator_has_next(list_iterator_segmentos))
			{
				segmento = list_iterator_next(list_iterator_segmentos);
				if (segmento->id == tcb_adm->segmento_nro)
				{
					segmento->id = contador_segmentos_libres--;
					segmento->estado = "LIBRE";
					elimine_segmento = true;
				}
				if (elimine_segmento)
				{
					break;
				}
			}
		}

		if (elimine_segmento)
		{
			list_remove(tabla_segmentos_tcb, index);
			elimine_tripulante = true;
		}
		else
		{
			index++;
		}
		if (elimine_tripulante)
		{
			break;
		}
	}
	list_iterator_destroy(list_iterator_tcb);
	list_iterator_destroy(list_iterator_segmentos);

	if (elimine_tripulante)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

uint32_t buscar_tripulante_no_asignado_segmentacion(int PCB_ID)
{
	if (PCB_ID > contador_patotas)
	{
		miLogInfo("La patota %d aún no ha sido iniciada", PCB_ID);
		return 99;
	}

	bool encontre_tripulante = false;
	u_int32_t posicion_tripulante;
	TCB *unTCB;
	TCB *TCB_encontrado;

	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos_tcb);
	TCB_adm *tcb_adm;

	while (list_iterator_has_next(list_iterator))
	{
		tcb_adm = list_iterator_next(list_iterator);

		if (tcb_adm->PID == PCB_ID)
		{
			unTCB = buscar_tripulante_segmentacion(tcb_adm->TID);

			if (unTCB->estado == 'N')
			{
				encontre_tripulante = true;
				TCB_encontrado = unTCB;
				break;
			}
		}
	}
	list_iterator_destroy(list_iterator);

	if (encontre_tripulante)
	{
		return TCB_encontrado;
	}
	else
	{
		miLogInfo("No existen tripulantes no iniciados para la patota %d", PCB_ID);
		return 99;
	}
}

//Crea la tabla de segmentos y el primer segmento vacio
void inicializar_segmentacion(int tamanio_memoria)
{
	contador_segmentos = 0;
	contador_segmentos_libres = -1;
	tabla_segmentos = list_create();
	tabla_segmentos_pcb = list_create();
	tabla_segmentos_tareas = list_create();
	tabla_segmentos_tcb = list_create();

	//Creo un segmento vacío del tamanio de la memoria
	Segmento *segmento_aux = malloc(sizeof(Segmento));
	segmento_aux->id = contador_segmentos_libres--;
	segmento_aux->dir_inicio = MEMORIA;
	segmento_aux->desplazamiento = tamanio_memoria;
	segmento_aux->estado = "LIBRE";

	//Agrego el segmento vacío a la tabla de segmentos
	list_add(tabla_segmentos, segmento_aux);
}


int inicializar_tripulante_segmentacion(int patota, char *unPunto, u_int32_t tareas)
{
	u_int32_t posicion_memoria = reservar_memoria(21);

	if (posicion_memoria == 99)
	{
		return -1;
	}

	TCB *miTCB = posicion_memoria;

	//sincronizar
	miTCB->TID = contador_tripulantes++;
	miTCB->estado = 'N';

	char **lista_puntos;
	lista_puntos = string_split(unPunto, "|");
	miTCB->pos_X = atoi(lista_puntos[0]);
	miTCB->pos_y = atoi(lista_puntos[1]);

	free(lista_puntos); 
	//linkear a tareas
	miTCB->proxima_instruccion = tareas;
	miTCB->PCB = buscar_patota(patota);

	//alta tripulante
	alta_tripulante(miTCB, patota);
}


int iniciar_patota_segmentacion(int cantidad_tripulantes, char *tareas, char *puntos)
{

	miLogInfo("Iniciando patota \n");
	u_int32_t posicion_memoria = reservar_memoria(sizeof(PCB));

	if (posicion_memoria == 99)
	{
		return -1;
	}

	PCB *unPCB = posicion_memoria;
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

	for (int i = 0; i < cantidad_tripulantes; i++)
	{

		//if (inicializar_tripulante(unPCB->PID, puntos[i], unPCB->Tareas) == -1)
		if (inicializar_tripulante_segmentacion(unPCB->PID, obtener_punto_string(puntos, i), unPCB->Tareas) == -1)
		{
			return -1;
		}
	}
	return unPCB->PID;
}


//Finaliza la segmentación
void finalizar_segmentacion()
{
	list_destroy(tabla_segmentos);
	list_destroy(tabla_segmentos_pcb);
	list_destroy(tabla_segmentos_tareas);
	list_destroy(tabla_segmentos_tcb);
}