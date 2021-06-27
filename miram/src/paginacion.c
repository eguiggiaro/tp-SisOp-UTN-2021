#include "paginacion.h"

void dump_memoria_paginacion_frames(bool mostrar_vacios)
{

	printf("--------------------------------------------------------------------\n");
	printf("Detalle tabla de frames\n");
	printf("--------------------------------------------------------------------\n");
	printf("Dir Inicio\tId frame\t Pagina\tEstado\tPatota\n");
	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);
	while (list_iterator_has_next(list_iterator))
	{
		Frame *un_frame = list_iterator_next(list_iterator);
		imprimir_frame(un_frame);
	}

	printf("--------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator);
}

//Imprime el frame

void imprimir_frame(Frame *un_frame)

{
	t_list_iterator *list_iterator_pcbs = list_iterator_create(tabla_pcbs);
	t_list_iterator *list_iterator_paginas;
	PCB_adm *un_pcb_adm;
	Pagina *una_pagina;
	bool encontre_patota = false;
	char *pagina_a_mostrar;
	char *patota_a_mostrar;

	if (strcmp(un_frame->estado, "OCUPADO") == 0)
	{
		while (list_iterator_has_next(list_iterator_pcbs))
		{
			un_pcb_adm = list_iterator_next(list_iterator_pcbs);

			list_iterator_paginas = list_iterator_create(un_pcb_adm->tabla_paginas);

			while (list_iterator_has_next(list_iterator_paginas))
			{
				una_pagina = list_iterator_next(list_iterator_paginas);
				if (una_pagina->id_frame == un_frame->id_frame)
				{
					encontre_patota = true;
					break;
				}

				if (encontre_patota)
				{
					break;
				}
			}
			list_iterator_destroy(list_iterator_paginas);
		}
	}
	if (encontre_patota)
	{
		patota_a_mostrar = string_itoa(un_pcb_adm->PID);
	}
	else
	{
		patota_a_mostrar = "-";
	}

	if (un_frame->id_pagina == -1)
	{
		pagina_a_mostrar = "-";
	}
	else
	{
		pagina_a_mostrar = string_itoa(una_pagina->id_pagina);
		
	}
	list_iterator_destroy(list_iterator_pcbs);


	printf("%d\t%d\t%s\t%s\t%s\n", un_frame->dir_inicio, un_frame->id_frame, pagina_a_mostrar, un_frame->estado, patota_a_mostrar);
}

//Muestra el contenido de la memoria
void dump_memoria_paginacion(bool mostrar_vacios)
{
	pthread_mutex_lock(&mutex_dump);
	dump_memoria_paginacion_frames(mostrar_vacios);
	fflush(stdout);
	pthread_mutex_unlock(&mutex_dump);
}

//Reserva memoria
u_int32_t reservar_memoria_paginacion(int bytes)
{
	/*
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
    */
}

int buscar_pagina(u_int32_t posicion_memoria)
{
	/*
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
    */
}

Pagina *buscar_pagina_por_id(int id_segmento)
{
	/*
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
    */
}

u_int32_t buscar_posicion_paginacion(int segmento)
{
	/*
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
    */
}

void alta_patota_paginacion(PCB *unPCB)
{
	/*
	PCB_adm *pcb_adm = malloc(sizeof(PCB_adm));
	pcb_adm->PID = unPCB->PID;
	pcb_adm->segmento_nro = buscar_segmento(unPCB);
	list_add(tabla_segmentos_pcb, pcb_adm);
    */
}

uint32_t buscar_patota_paginacion(int PID)
{
	/*
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
    */
}

void alta_tripulante_paginacion(TCB *unTCB, int patota)
{
	/*
	TCB_adm *tcb_adm = malloc(sizeof(TCB_adm));
	tcb_adm->TID = unTCB->TID;
	tcb_adm->PID = patota;
	tcb_adm->segmento_nro = buscar_segmento(unTCB);
	list_add(tabla_segmentos_tcb, tcb_adm);
    */
}

void alta_tareas_paginacion(int PCB_ID, char *tareas)
{
	/*
	Tarea_adm *tareas_adm = malloc(sizeof(Tarea_adm));
	tareas_adm->PID = PCB_ID;
	tareas_adm->segmento_nro = buscar_segmento(tareas);
	list_add(tabla_segmentos_tareas, tareas_adm);
    */
}

uint32_t buscar_tripulante_paginacion(int TCB_ID)
{
	/*
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
    */
}

int expulsar_tripulante_paginacion(int tripulante_id)
{
	/*
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
    */
}

uint32_t buscar_tripulante_no_asignado_paginacion(int PCB_ID)
{
	/*
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
    */
}


Frame *buscar_frame_libre()
{

	Frame *un_frame_libre;
	Frame *un_frame;
	bool encontre_frame = false;

	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

	while (list_iterator_has_next(list_iterator))
	{
		un_frame = list_iterator_next(list_iterator);
		if (strcmp(un_frame->estado, "LIBRE") == 0)
		{
			un_frame_libre = un_frame;
			encontre_frame = true;
			break;
		}
	}
	list_iterator_destroy(list_iterator);

	if (encontre_frame)
	{
		return un_frame_libre;
	}
	else
	{
		return -1;
	}
}

Frame *buscar_frame(int id_frame)
{

	Frame *un_frame;
	bool encontre_frame = false;

	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

	while (list_iterator_has_next(list_iterator))
	{
		un_frame = list_iterator_next(list_iterator);
		if (un_frame->id_frame == id_frame)
		{
			encontre_frame = true;
			break;
		}
	}
	list_iterator_destroy(list_iterator);

	if (encontre_frame)
	{
		return un_frame;
	}
	else
	{
		return -1;
	}
}

int guardar_pagina_en_frame(int id_frame, void *pagina, int id_pagina)
{

	Frame *un_frame;
	bool encontre_frame = false;

	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

	while (list_iterator_has_next(list_iterator))
	{
		un_frame = list_iterator_next(list_iterator);

		if (un_frame->id_frame == id_frame)
		{
			memcpy(un_frame->dir_inicio, pagina, tamanio_pagina_paginacion);
			encontre_frame = true;
			un_frame->estado = "OCUPADO";
			un_frame->id_pagina = id_pagina;
			break;
		}
	}
	list_iterator_destroy(list_iterator);
	dump_memoria_paginacion(true);

	if (encontre_frame)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int guardar_patota(PCB *un_pcb, PCB_adm *un_pcb_adm)
{

	int cantidad_paginas = un_pcb_adm->cantidad_paginas_pcb;
	Pagina *una_pagina;
	Frame *un_frame;

	for (int i = 0; i < cantidad_paginas; i++)
	{
		una_pagina = list_get(un_pcb_adm->tabla_paginas, i);
		un_frame = buscar_frame(una_pagina->id_frame);
		if (i + 1 == cantidad_paginas)
		{
			memcpy(un_frame->dir_inicio, (void *)un_pcb, tamanio_pagina_paginacion - un_pcb_adm->offset_tareas);
		}
		else
		{
			memcpy(un_frame->dir_inicio, (void *)un_pcb, tamanio_pagina_paginacion);
		}
	}
}

int iniciar_patota_paginacion( int cantidad_tripulantes, char *tareas, char *puntos)
{
	int tamanio_alocado = 0;
	Pagina *una_pagina;
	PCB_adm *pcb_adm = malloc(sizeof(PCB_adm));
	pcb_adm->PID = contador_patotas++;
	pcb_adm->tabla_paginas = list_create();
	pcb_adm->cantidad_paginas_pcb = 0;
	pcb_adm->cantidad_paginas_tareas = 0;

	int tamanio_pcb = 8;
	int tamanio_tareas = strlen(tareas);
	int tamanio_tcb = 21;

	void *pagina = malloc(tamanio_pagina_paginacion);
	PCB *un_pcb = malloc(sizeof(PCB));
	TCB *un_tcb;
	TCB_adm *un_tcb_adm;

	Frame *un_frame_libre;
	int offset;
	char **lista_puntos;

	bool ubique_tareas = false;
	bool liberar_pagina = false;
	bool ubique_tripulante = false;

	char *un_punto;

	un_pcb->PID = contador_patotas++;

	//Cargo la patota
	while (tamanio_alocado < tamanio_pcb)
	{
		//CUIDADOOOO ESTA PRIMER APARTE VA EN UN IF AFUEREA

		un_frame_libre = buscar_frame_libre();
		una_pagina = malloc(sizeof(Pagina));
		una_pagina->id_pagina = contador_paginas++;
		una_pagina->id_frame = un_frame_libre->id_frame;
		una_pagina->estado = "OCUPADO";
		pcb_adm->cantidad_paginas_pcb++;
		pcb_adm->pagina_inicio_pcb = una_pagina->id_pagina;
		list_add(tabla_pcbs, pcb_adm);
		list_add(pcb_adm->tabla_paginas, una_pagina);

		if ((tamanio_pcb - tamanio_alocado) <= tamanio_pagina_paginacion)
		{
			if ((tamanio_pcb - tamanio_alocado) == tamanio_pagina_paginacion)
			{
				offset = 0;
				liberar_pagina = true;
			}
			else
			{
				offset = tamanio_pagina_paginacion - (tamanio_pagina_paginacion - (tamanio_pcb - tamanio_alocado)) + 1;
			}
			memcpy(pagina, (void *)un_pcb + tamanio_alocado, tamanio_pcb - tamanio_alocado);
			tamanio_alocado = tamanio_pcb;
		}
		else
		{
			tamanio_alocado += tamanio_pagina_paginacion;
			memcpy(pagina, (void *)un_pcb + tamanio_alocado, tamanio_pagina_paginacion);
			liberar_pagina = true;
		}

		guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
		if (liberar_pagina)
		{
			free(pagina);
			pagina = malloc(tamanio_pagina_paginacion);
			liberar_pagina = false;
		}
	}

	liberar_pagina = false;
	tamanio_alocado = 0;
	pcb_adm->offset_tareas = offset;
	//Cargo las tareas

	if (offset > 0)
	{
		pcb_adm->pagina_inicio_tareas = una_pagina->id_pagina;

		if ((tamanio_tareas) <= (tamanio_pagina_paginacion - offset + 1))
		{
			memcpy(pagina + offset, (void *)tareas + tamanio_alocado, tamanio_tareas);
			if ((tamanio_tareas) == (tamanio_pagina_paginacion - offset + 1))
			{
				offset = 0;
				liberar_pagina = true;
			}
			else
			{
				offset = offset + (tamanio_tareas);
			}
			tamanio_alocado = tamanio_tareas;
			ubique_tareas = true;
			pcb_adm->cantidad_paginas_tareas++;
		}
		else
		{
			tamanio_alocado += tamanio_pagina_paginacion;
			memcpy(pagina + offset, (void *)tareas, tamanio_pagina_paginacion - offset + 1);
			liberar_pagina = true;
			pcb_adm->cantidad_paginas_tareas++;
		}
		guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
		if (liberar_pagina)
		{
			free(pagina);
			pagina = malloc(tamanio_pagina_paginacion);
			liberar_pagina = false;
		}
	}

	if (!ubique_tareas)
	{

		while (tamanio_alocado < tamanio_tareas)
		{

			un_frame_libre = buscar_frame_libre();
			una_pagina = malloc(sizeof(Pagina));
			una_pagina->id_pagina = contador_paginas++;
			una_pagina->id_frame = un_frame_libre->id_frame;
			una_pagina->estado = "OCUPADO";
			list_add(pcb_adm->tabla_paginas, una_pagina);
			pcb_adm->cantidad_paginas_tareas++;

			if ((tamanio_tareas - tamanio_alocado) <= tamanio_pagina_paginacion)
			{
				if ((tamanio_tareas - tamanio_alocado) == tamanio_pagina_paginacion)
				{
					offset = 0;
					liberar_pagina = true;
				}
				else
				{
					offset = tamanio_pagina_paginacion - (tamanio_tareas - tamanio_alocado) + 1;
				}
				memcpy(pagina, (void *)tareas + tamanio_alocado, tamanio_tareas - tamanio_alocado);
				tamanio_alocado = tamanio_tareas;
			}
			else
			{
				tamanio_alocado += tamanio_pagina_paginacion;
				memcpy(pagina, (void *)tareas + tamanio_alocado, tamanio_pagina_paginacion);
				liberar_pagina = true;
			}

			guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
			if (liberar_pagina)
			{
				free(pagina);
				pagina = malloc(tamanio_pagina_paginacion);
				liberar_pagina = false;
			}
		}
	}

	un_pcb->Tareas = pcb_adm->pagina_inicio_tareas;
	guardar_patota(un_pcb, pcb_adm);
	liberar_pagina = false;
	tamanio_alocado = 0;

	// Cargo tripulantes

	for (int i = 0; i < cantidad_tripulantes; i++)

	{
		un_tcb_adm = malloc(sizeof(TCB_adm));
		un_tcb_adm->TID = contador_patotas++;
		un_tcb_adm->cantidad_paginas = 0;

		un_tcb = malloc(sizeof(TCB));
		un_tcb->estado = 'N';
		un_tcb->PCB = pcb_adm->pagina_inicio_pcb;

		un_punto = obtener_punto_string(puntos, i);
		lista_puntos = string_split(un_punto, "|");
		un_tcb->pos_X = atoi(lista_puntos[0]);
		un_tcb->pos_y = atoi(lista_puntos[1]);
		free(lista_puntos);

		un_tcb->proxima_instruccion = 1;

		if (offset > 0)
		{
			un_tcb_adm->pagina_inicio = una_pagina->id_pagina;
			un_tcb_adm->offset = offset;
			un_tcb_adm->cantidad_paginas++;
			list_add(pcb_adm->tabla_TCB_admin, un_tcb_adm);

			if ((tamanio_tcb) <= (tamanio_pagina_paginacion - offset + 1))
			{
				memcpy(pagina + offset, (void *)un_tcb + tamanio_alocado, tamanio_tcb);
				if ((tamanio_tcb) == (tamanio_pagina_paginacion - offset + 1))
				{
					offset = 0;
					liberar_pagina = true;
				}
				else
				{
					offset = offset + (tamanio_tcb);
				}
				tamanio_alocado = tamanio_tcb;
				ubique_tripulante = true;
			}
			else
			{
				tamanio_alocado += tamanio_pagina_paginacion;
				memcpy(pagina + offset, (void *)un_tcb, tamanio_pagina_paginacion - offset + 1);
				liberar_pagina = true;
			}
			guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
			if (liberar_pagina)
			{
				free(pagina);
				pagina = malloc(tamanio_pagina_paginacion);
				liberar_pagina = false;
			}
		}

		if (!ubique_tripulante)
		{

			while (tamanio_alocado < tamanio_tcb)
			{

				un_frame_libre = buscar_frame_libre();
				una_pagina = malloc(sizeof(Pagina));
				una_pagina->id_pagina = contador_paginas++;
				una_pagina->id_frame = un_frame_libre->id_frame;
				una_pagina->estado = "OCUPADO";
				list_add(pcb_adm->tabla_paginas, una_pagina);

				if ((tamanio_tcb - tamanio_alocado) <= tamanio_pagina_paginacion)
				{
					if ((tamanio_tcb - tamanio_alocado) == tamanio_pagina_paginacion)
					{
						offset = 0;
						liberar_pagina = true;
					}
					else
					{
						offset = tamanio_pagina_paginacion - (tamanio_tcb - tamanio_alocado) + 1;
					}
					memcpy(pagina, (void *)un_tcb + tamanio_alocado, tamanio_tcb - tamanio_alocado);
					tamanio_alocado = tamanio_tcb;
				}
				else
				{
					tamanio_alocado += tamanio_pagina_paginacion;
					memcpy(pagina, (void *)un_tcb + tamanio_alocado, tamanio_pagina_paginacion);
					liberar_pagina = true;
				}

				guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
				if (liberar_pagina)
				{
					free(pagina);
					pagina = malloc(tamanio_pagina_paginacion);
					liberar_pagina = false;
				}
			}
		}

		tamanio_alocado = 0;
		liberar_pagina = false;
	}

	return pcb_adm->PID;
}

void inicializar_paginacion(int tamanio_memoria, int tamanio_pagina)
{
	contador_frames = tamanio_memoria / tamanio_pagina;
	contador_frames_libres = contador_frames;
	contador_paginas = 0;
	tamanio_pagina_paginacion = tamanio_pagina;
	Frame *un_frame;
	tabla_frames = list_create();
	tabla_pcbs = list_create();
	tabla_tareas = list_create();
	tabla_tcbs = list_create();

	for (int i = 0; i < contador_frames; i++)
	{
		un_frame = malloc(sizeof(Frame));
		un_frame->id_frame = i + 1;
		un_frame->estado = "LIBRE";
		un_frame->dir_inicio = MEMORIA + (i * tamanio_pagina);
		un_frame->id_pagina = -1;
		list_add(tabla_frames, un_frame);
	}
}

//Finaliza la segmentación
void finalizar_paginacion()
{
	list_destroy(tabla_frames);
}