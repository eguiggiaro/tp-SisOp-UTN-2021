#include "paginacion.h"

//Muestra el contenido de la memoria
void archivo_paginacion(bool mostrar_vacios)
{
	pthread_mutex_lock(&mutex_dump);

	char *nombre_archivo = string_new();
	string_append(&nombre_archivo, "dmp/Dump_");
	string_append(&nombre_archivo, temporal_get_string_time("%d_%m_%y_%H:%M:%S"));
	string_append(&nombre_archivo, ".dmp");

	FILE *archivoDump = fopen(nombre_archivo, "w+");
	dump_memoria_paginacion_frames_archivo(mostrar_vacios, archivoDump);
	fflush(stdout);
	fclose(archivoDump);
	pthread_mutex_unlock(&mutex_dump);
}

void dump_memoria_paginacion_frames_archivo(bool mostrar_vacios, FILE *archivoDump)
{

	fprintf(archivoDump, "-------------------------------------------------------------------------------\n");
	fprintf(archivoDump, "Detalle tabla de frames\n");
	fprintf(archivoDump, "-------------------------------------------------------------------------------\n");
	fprintf(archivoDump, "Dir Inicio\tId frame\tPagina\tLRU\tCLOCK\tEstado\tPatota\tContenido\n");
	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);
	while (list_iterator_has_next(list_iterator))
	{
		Frame *un_frame = list_iterator_next(list_iterator);
		imprimir_frame_archivo(un_frame, archivoDump);
	}

	fprintf(archivoDump, "-------------------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator);
}

void imprimir_frame_archivo(Frame *un_frame, FILE *archivoDump)

{
	t_list_iterator *list_iterator_pcbs = list_iterator_create(tabla_pcbs);
	t_list_iterator *list_iterator_paginas;
	t_list_iterator *list_iterator_contenido;
	PCB_adm *un_pcb_adm;
	Pagina *una_pagina;
	bool encontre_patota = false;
	char *pagina_a_mostrar;
	char *patota_a_mostrar;
	int LRU = -1;
	int clock = -1;
	char *contenido = string_new();
	char *auxiliar;

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
					list_iterator_contenido = list_iterator_create(una_pagina->contenido);

					while (list_iterator_has_next(list_iterator_contenido))
					{
						auxiliar = list_iterator_next(list_iterator_contenido);
						string_append(&contenido, auxiliar);
						string_append(&contenido, "|");
					}

					list_iterator_destroy(list_iterator_contenido);

					encontre_patota = true;
					break;
				}

				if (encontre_patota)
				{
					break;
				}
			}
			list_iterator_destroy(list_iterator_paginas);
			if (encontre_patota)
			{
				break;
			}
		}
	}
	if (encontre_patota)
	{
		patota_a_mostrar = string_itoa(un_pcb_adm->PID);
	}
	else
	{
		patota_a_mostrar = "-";
		string_append(&contenido, "-");
	}

	if (un_frame->id_pagina == -1)
	{
		pagina_a_mostrar = "-";
	}
	else
	{
		pagina_a_mostrar = string_itoa(una_pagina->id_pagina);
		LRU = una_pagina->LRU;
		clock = una_pagina->clock;
	}
	list_iterator_destroy(list_iterator_pcbs);

	fprintf(archivoDump, "%d\t%d\t\t%s\t%d\t%d\t%s\t%s\t%s\n", un_frame->dir_inicio, un_frame->id_frame, pagina_a_mostrar, LRU, clock, un_frame->estado, patota_a_mostrar, contenido);
	free(contenido);
}

void dump_memoria_paginacion_frames(bool mostrar_vacios)
{

	printf("-------------------------------------------------------------------------------\n");
	printf("Detalle tabla de frames\n");
	printf("-------------------------------------------------------------------------------\n");
	printf("Dir Inicio\tId frame\tPagina\tLRU\tCLOCK\tEstado\tPatota\tContenido\n");
	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);
	while (list_iterator_has_next(list_iterator))
	{
		Frame *un_frame = list_iterator_next(list_iterator);
		imprimir_frame(un_frame);
	}

	printf("-------------------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator);
}

//Imprime el frame

void imprimir_frame(Frame *un_frame)

{
	t_list_iterator *list_iterator_pcbs = list_iterator_create(tabla_pcbs);
	t_list_iterator *list_iterator_paginas;
	t_list_iterator *list_iterator_contenido;
	PCB_adm *un_pcb_adm;
	Pagina *una_pagina;
	bool encontre_patota = false;
	char *pagina_a_mostrar;
	char *patota_a_mostrar;
	char *contenido = string_new();
	char *auxiliar;
	int LRU = -1;
	int clock = -1;

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
					list_iterator_contenido = list_iterator_create(una_pagina->contenido);

					while (list_iterator_has_next(list_iterator_contenido))
					{
						auxiliar = list_iterator_next(list_iterator_contenido);
						string_append(&contenido, auxiliar);
						string_append(&contenido, "|");
					}

					list_iterator_destroy(list_iterator_contenido);

					encontre_patota = true;
					break;
				}

				if (encontre_patota)
				{
					break;
				}
			}
			list_iterator_destroy(list_iterator_paginas);
			if (encontre_patota)
			{
				break;
			}
		}
	}
	if (encontre_patota)
	{
		patota_a_mostrar = string_itoa(un_pcb_adm->PID);
	}
	else
	{
		patota_a_mostrar = "-";
		string_append(&contenido, "-");
	}

	if (un_frame->id_pagina == -1)
	{
		pagina_a_mostrar = "-";
	}
	else
	{
		pagina_a_mostrar = string_itoa(una_pagina->id_pagina);
		LRU = una_pagina->LRU;
		clock = una_pagina->clock;
	}
	list_iterator_destroy(list_iterator_pcbs);

	printf("%d\t%d\t\t%s\t%d\t%d\t%s\t%s\t%s\n", un_frame->dir_inicio, un_frame->id_frame, pagina_a_mostrar, LRU,clock, un_frame->estado, patota_a_mostrar, contenido);
	free(contenido);
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
    */
}

int buscar_pagina(u_int32_t posicion_memoria)
{
	/*
   */
}

u_int32_t buscar_posicion_paginacion(int segmento)
{
	/*
   */
}

void alta_patota_paginacion(PCB *unPCB)
{
	/*
    */
}

uint32_t buscar_patota_paginacion(int PID)
{
	/*
    */
}

void alta_tareas_paginacion(int PCB_ID, char *tareas)
{
	/*
    */
}

uint32_t buscar_tripulante_paginacion(int TCB_ID)
{
	/*
    */
}

int expulsar_tripulante_paginacion(int tripulante_id)
{
	TCB *unTCB = buscar_tcb_por_id(tripulante_id);
	TCB_adm *tcb_adm;
	char *identificador = string_new();
	string_append(&identificador, "T");
	string_append(&identificador, string_itoa(tripulante_id));

	PCB_adm *pcb_adm = buscar_patota_tripulante(tripulante_id);
	t_list_iterator *iterador_tripulantes = list_iterator_create(pcb_adm->tabla_TCB_admin);
	while (list_iterator_has_next(iterador_tripulantes))
	{
		tcb_adm = list_iterator_next(iterador_tripulantes);

		if (tcb_adm->TID == tripulante_id)
		{
			break;
		}
	}
	list_iterator_destroy(iterador_tripulantes);

	Pagina *una_pagina;
	char *contenido_pagina;
	Frame *un_frame;
	bool eliminar_pagina = false;

	t_list_iterator *iterador_paginas = list_iterator_create(pcb_adm->tabla_paginas);
	t_list_iterator *iterador_contenido;
	while (list_iterator_has_next(iterador_paginas))
	{
		una_pagina = list_iterator_next(iterador_paginas);
		iterador_contenido = list_iterator_create(una_pagina->contenido);

		while (list_iterator_has_next(iterador_contenido))
		{
			contenido_pagina = list_iterator_next(iterador_contenido);
			if (strcmp(contenido_pagina, identificador) == 0)
			{
				if (list_size(una_pagina->contenido) == 1)
				{
					eliminar_pagina = true;
				}
				list_iterator_remove(iterador_contenido);
				break;
			}
		}
		list_iterator_destroy(iterador_contenido);

		if (eliminar_pagina)
		{
			un_frame = buscar_frame_por_pagina(una_pagina->id_pagina);
			un_frame->estado = "LIBRE";
			un_frame->id_pagina = -1;
			list_iterator_remove(iterador_paginas);
			eliminar_pagina = false;
		}
	}

	list_iterator_destroy(iterador_paginas);

	tcb_adm->cantidad_paginas = 0;

	free(identificador);
	return 1;
}

PCB_adm *buscar_patota_tripulante(int tripulante_id)
{
	t_list_iterator *iterador_pcbs = list_iterator_create(tabla_pcbs);
	t_list_iterator *iterador_tcbs;
	PCB_adm *pcb_adm;
	TCB_adm *tcb_adm;
	bool encontre_patota = false;

	while (list_iterator_has_next(iterador_pcbs))
	{
		pcb_adm = list_iterator_next(iterador_pcbs);

		iterador_tcbs = list_iterator_create(pcb_adm->tabla_TCB_admin);

		while (list_iterator_has_next(iterador_tcbs))

		{
			tcb_adm = list_iterator_next(iterador_tcbs);
			if (tcb_adm->TID == tripulante_id)
			{
				encontre_patota = true;
				break;
			}
		}

		list_iterator_destroy(iterador_tcbs);

		if (encontre_patota)
		{
			break;
		}
	}
	list_iterator_destroy(iterador_pcbs);

	return pcb_adm;
}

char *buscar_posicion_tripulante_paginacion(int tripulante_id)
{

	TCB *unTCB = buscar_tcb_por_id(tripulante_id);

	char *posicion_tripulante;

	posicion_tripulante = string_itoa(unTCB->pos_X);
	string_append(&posicion_tripulante, "|");
	string_append(&posicion_tripulante, string_itoa(unTCB->pos_y));

	return posicion_tripulante;
}

char *proxima_tarea_tripulante_paginacion(int tripulante_id)
{
	TCB *unTCB = buscar_tcb_por_id(tripulante_id);
	int index = 0;
	char *string_tarea = string_new();
	int contador_tareas = 0;
	char caracter;
	PCB_adm *pcb_adm = buscar_patota_tripulante(tripulante_id);
	char *tareas = buscar_tareas(pcb_adm->PID);

	if (unTCB->proxima_instruccion == 99)
	{
		return "$";
	}

	while (1)
	{
		caracter = tareas[index];

		if (caracter == '$')
		{
			unTCB->proxima_instruccion = 99;
			break;
		}

		if (caracter == '|')
		{
			if (contador_tareas == unTCB->proxima_instruccion)
			{
				unTCB->proxima_instruccion++;
				string_append(&string_tarea, "|");
				break;
			}
			else
			{
				free(string_tarea);
				string_tarea = string_new();
				contador_tareas++;
			}
		}
		else
		{
			string_append_with_format(&string_tarea, "%c", caracter);
		}

		index++;
	}

	guardar_tripulante(unTCB, pcb_adm);

	return string_tarea;
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
			un_frame_libre->estado = "OCUPADO";
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
		return hacer_lugar_memoria();
	}
}

Frame *hacer_lugar_memoria()
{

	Frame *un_frame_candidato;
	Pagina *una_pagina_candidata;
	Frame *un_frame;
	bool encontre_frame = false;
	int minimo_LRU = contador_LRU + 100;
	Pagina *una_pagina;
	int pagina_candidata;

	if (strcmp(algoritmo_reemplazo, "LRU") == 0)
	{

		t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

		while (list_iterator_has_next(list_iterator))
		{
			un_frame = list_iterator_next(list_iterator);
			if (strcmp(un_frame->estado, "OCUPADO") == 0)
			{
				una_pagina = buscar_pagina_por_id(un_frame->id_pagina);

				if (una_pagina->LRU < minimo_LRU)
				{
					minimo_LRU = una_pagina->LRU;
					un_frame_candidato = un_frame;
					una_pagina_candidata = una_pagina;
				}
			}
		}
		list_iterator_destroy(list_iterator);
	}
	else
	{

		while (!queue_is_empty(cola_FIFO_clock))
		{
			pagina_candidata = queue_pop(cola_FIFO_clock);
			una_pagina = buscar_pagina_por_id(pagina_candidata);

			if (una_pagina->clock == 1)
			{
				una_pagina->clock = 0;
				queue_push(cola_FIFO_clock, pagina_candidata);
			}
			else
			{
				una_pagina->clock = 1;
				una_pagina_candidata = una_pagina;
				un_frame_candidato = buscar_frame_por_pagina(pagina_candidata);
				break;
			}
		}
	}
	llevar_a_swap(un_frame_candidato, una_pagina_candidata);

	una_pagina_candidata->en_memoria = false;
	una_pagina_candidata->LRU = 0;
	una_pagina_candidata->id_frame = -1;

	un_frame_candidato->id_pagina = -1;

	dump_memoria_paginacion(true);

	return un_frame_candidato;
}

void *llevar_a_swap(Frame *un_frame, Pagina *una_pagina)
{
	int espacio_a_ocupar;
	bool encontre_posicion_SWAP = false;

	for (int i = 0; i < contador_espacios_SWAP; i++)
	{
		if (bitarray_test_bit(bitmap, i) == 0)
		{
			bitarray_set_bit(bitmap, i); //Asigno el bloque libre (pongo en 1 el bit).
			encontre_posicion_SWAP = true;
			espacio_a_ocupar = i;
			break;
		}
	}

	if (encontre_posicion_SWAP)
	{
		fseek(SWAP, tamanio_pagina_paginacion * espacio_a_ocupar, SEEK_SET);
		fwrite(un_frame->dir_inicio, tamanio_pagina_paginacion, 1, SWAP);
	}

	Elemento_SWAP *elemento_swap = malloc(sizeof(Elemento_SWAP));
	elemento_swap->id_pagina = una_pagina->id_pagina;
	elemento_swap->posicion_SWAP = espacio_a_ocupar;

	list_add(tabla_elementos_SWAP, elemento_swap);
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

Frame *buscar_frame_por_pagina(int id_pagina)
{

	Frame *un_frame;
	bool encontre_frame = false;

	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

	while (list_iterator_has_next(list_iterator))
	{
		un_frame = list_iterator_next(list_iterator);
		if (un_frame->id_pagina == id_pagina)
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
		un_frame = buscar_frame_libre();
		return traer_de_SWAP(id_pagina, un_frame);
	}
}

Frame *traer_de_SWAP(int id_pagina, Frame *un_frame)
{
	t_list_iterator *iterador_SWAP = list_iterator_create(tabla_elementos_SWAP);
	Elemento_SWAP *elemento_swap;
	Pagina *una_pagina = buscar_pagina_por_id(id_pagina);

	while (list_iterator_has_next(iterador_SWAP))
	{
		elemento_swap = list_iterator_next(iterador_SWAP);

		if (elemento_swap->id_pagina == id_pagina)
		{
			un_frame->id_pagina = id_pagina;
			una_pagina->id_frame = un_frame->id_frame;
			una_pagina->en_memoria = true;
			una_pagina->estado = "OCUPADO";
			una_pagina->clock = 1;
			queue_push(cola_FIFO_clock,id_pagina);

			fseek(SWAP, tamanio_pagina_paginacion * elemento_swap->posicion_SWAP, SEEK_SET);
			fread(un_frame->dir_inicio, tamanio_pagina_paginacion, 1, SWAP);

			bitarray_clean_bit(bitmap, elemento_swap->posicion_SWAP);

			list_iterator_remove(iterador_SWAP);
			break;
		}
	}
	list_iterator_destroy(iterador_SWAP);

	dump_memoria_paginacion(true);

	return un_frame;
}

int guardar_pagina_en_frame(int id_frame, void *pagina, int id_pagina)
{

	Frame *un_frame;
	bool encontre_frame = false;
	Pagina *una_pagina;

	t_list_iterator *list_iterator = list_iterator_create(tabla_frames);

	while (list_iterator_has_next(list_iterator))
	{
		un_frame = list_iterator_next(list_iterator);

		if (un_frame->id_frame == id_frame)
		{
			memcpy(un_frame->dir_inicio, pagina, tamanio_pagina_paginacion);
			encontre_frame = true;
			un_frame->id_pagina = id_pagina;

			una_pagina = buscar_pagina_por_id(un_frame->id_pagina);
			una_pagina->LRU = contador_LRU++;

			if (una_pagina->clock == -1)
			{
				queue_push(cola_FIFO_clock, una_pagina->id_pagina);
				una_pagina->clock = 1;
			}

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
	int tamanio_alocado = 0;
	int tamanio_pcb = 8;

	for (int i = 0; i < cantidad_paginas; i++)
	{
		una_pagina = list_get(un_pcb_adm->tabla_paginas, i);
		una_pagina->LRU = contador_LRU++;
		un_frame = buscar_frame_por_pagina(una_pagina->id_pagina);
		if (i + 1 == cantidad_paginas)
		{
			memcpy(un_frame->dir_inicio, (void *)un_pcb, tamanio_pcb - tamanio_alocado);
			tamanio_alocado = tamanio_pcb;
		}
		else
		{
			memcpy(un_frame->dir_inicio, (void *)un_pcb, tamanio_pagina_paginacion);
			tamanio_alocado += tamanio_pagina_paginacion;
		}
	}
}

int guardar_tripulante(TCB *un_tcb, PCB_adm *un_pcb_adm)
{
	TCB_adm *un_tcb_adm;
	t_list_iterator *iterador_tripulantes = list_iterator_create(un_pcb_adm->tabla_TCB_admin);

	while (list_iterator_has_next(iterador_tripulantes))
	{
		un_tcb_adm = list_iterator_next(iterador_tripulantes);
		if (un_tcb_adm->TID == un_tcb->TID)
		{
			break;
		}
	}

	list_iterator_destroy(iterador_tripulantes);

	int cantidad_paginas = un_tcb_adm->cantidad_paginas;
	int una_pagina;
	int tamanio_alocado = 0;
	int tamanio_tcb = 21;
	int tamanio_a_guardar = 0;
	int tamanio_guardado = 0;
	int limite_buffer = 0;
	int offset = un_tcb_adm->offset;
	Pagina *pagina;

	Frame *un_frame = buscar_frame_por_pagina(un_tcb_adm->pagina_inicio);
	una_pagina = un_tcb_adm->pagina_inicio;

	for (int i = 0; i < cantidad_paginas; i++)
	{
		pagina = buscar_pagina_por_id(una_pagina);
		pagina->LRU = contador_LRU++;

		if (i == 0)
		{
			if (tamanio_pagina_paginacion - offset > tamanio_tcb - tamanio_guardado)
			{
				tamanio_a_guardar = tamanio_tcb - tamanio_guardado;
				tamanio_guardado += tamanio_a_guardar;
			}
			else
			{
				tamanio_a_guardar = tamanio_pagina_paginacion - offset;
				tamanio_guardado += tamanio_a_guardar;
			}

			memcpy(un_frame->dir_inicio + offset, (void *)un_tcb + limite_buffer, tamanio_a_guardar);
			limite_buffer = tamanio_a_guardar;
		}
		else
		{
			if (tamanio_pagina_paginacion > tamanio_tcb - tamanio_guardado)
			{
				tamanio_a_guardar = tamanio_tcb - tamanio_guardado;
				tamanio_guardado += tamanio_a_guardar;
			}
			else
			{
				tamanio_a_guardar = tamanio_pagina_paginacion;
				tamanio_guardado += tamanio_a_guardar;
			}

			memcpy(un_frame->dir_inicio, (void *)un_tcb + limite_buffer, tamanio_a_guardar);

			limite_buffer = tamanio_guardado;
		}

		if (i + 1 != cantidad_paginas)
		{
			una_pagina = siguiente_pagina(una_pagina, un_pcb_adm);
			un_frame = buscar_frame_por_pagina(una_pagina);
		}
	}
}

PCB_adm *buscar_patota_adm(int patota_id)
{

	PCB_adm *un_pcb_adm;
	bool encontre_patota = false;

	t_list_iterator *list_iterator = list_iterator_create(tabla_pcbs);

	while (list_iterator_has_next(list_iterator))
	{
		un_pcb_adm = list_iterator_next(list_iterator);

		if (un_pcb_adm->PID == patota_id)
		{
			encontre_patota = true;
			break;
		}
	}
	list_iterator_destroy(list_iterator);

	if (encontre_patota)
	{
		return un_pcb_adm;
	}
	else
	{
		return -1;
	}
}

int siguiente_pagina(int pagina_actual, PCB_adm *pcb_adm)
{

	bool encontre_pagina_actual = false;
	int pagina_retorno;
	Pagina *una_pagina;

	t_list_iterator *list_iterator_tabla;

	list_iterator_tabla = list_iterator_create(pcb_adm->tabla_paginas);

	while (list_iterator_has_next(list_iterator_tabla))
	{
		una_pagina = list_iterator_next(list_iterator_tabla);
		if (encontre_pagina_actual)
		{
			pagina_retorno = una_pagina->id_pagina;
			break;
		}

		if (una_pagina->id_pagina == pagina_actual)
		{
			encontre_pagina_actual = true;
		}
	}
	free(list_iterator_tabla);
	return pagina_retorno;
}

TCB *buscar_tcb(PCB_adm *pcb_adm, int tripulante_posicion)
{
	if (list_size(pcb_adm->tabla_TCB_admin) < tripulante_posicion)
	{
		return -1;
	}

	TCB_adm *tcb_adm = list_get(pcb_adm->tabla_TCB_admin, tripulante_posicion);
	int cantidad_paginas = tcb_adm->cantidad_paginas;
	int pagina = tcb_adm->pagina_inicio;
	int tamanio_recuperado = 0;
	int tamanio_a_recuperar = 0;
	int offset = tcb_adm->offset;
	int limite_buffer = 0;
	int tamanio_tcb = 21;
	void *buffer = malloc(sizeof(TCB));
	TCB *tcb_retorno;

	Frame *un_frame = buscar_frame_por_pagina(tcb_adm->pagina_inicio);
	Pagina *una_pagina;

	for (int i = 0; i < cantidad_paginas; i++)
	{
		una_pagina = buscar_pagina_por_id(pagina);
		una_pagina->LRU = contador_LRU++;

		if (i == 0)
		{
			if (tamanio_pagina_paginacion - offset > tamanio_tcb - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_tcb - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion - offset;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio + offset, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		else
		{
			if (tamanio_pagina_paginacion > tamanio_tcb - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_tcb - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		if (i + 1 != cantidad_paginas)
		{
			pagina = siguiente_pagina(pagina, pcb_adm);
			un_frame = buscar_frame_por_pagina(pagina);
		}
	}

	tcb_retorno = buffer;

	return tcb_retorno;
}

TCB *buscar_tcb_por_id(int tripulante_id)
{
	PCB_adm *pcb_adm;
	TCB_adm *tcb_adm;
	int tripulante_posicion = 0;
	bool encontre_tripulante = false;

	t_list_iterator *iterador_patotas = list_iterator_create(tabla_pcbs);
	t_list_iterator *iterador_tripulantes;

	while (list_iterator_has_next(iterador_patotas))
	{
		pcb_adm = list_iterator_next(iterador_patotas);

		iterador_tripulantes = list_iterator_create(pcb_adm->tabla_TCB_admin);

		while (list_iterator_has_next(iterador_tripulantes))
		{
			tcb_adm = list_iterator_next(iterador_tripulantes);
			if (tcb_adm->TID == tripulante_id)
			{
				encontre_tripulante = true;
				break;
			}

			tripulante_posicion++;
		}

		list_iterator_destroy(iterador_tripulantes);

		if (encontre_tripulante)
		{
			break;
		}
		tripulante_posicion = 0;
	}

	list_iterator_destroy(iterador_patotas);

	return buscar_tcb(pcb_adm, tripulante_posicion);
}

PCB *buscar_PCB(PCB_adm *pcb_adm)
{
	int cantidad_paginas = pcb_adm->cantidad_paginas_pcb;
	int pagina = pcb_adm->pagina_inicio_pcb;
	int offset = 0;
	PCB *un_PCB = malloc(sizeof(PCB));
	int tamanio_recuperado = 0;
	int tamanio_a_recuperar = 0;
	int tamanio_pcb = 8;
	int limite_buffer = 0;
	void *buffer = malloc(tamanio_pcb);
	Pagina *una_pagina;

	Frame *un_frame = buscar_frame_por_pagina(pagina);

	for (int i = 0; i < cantidad_paginas; i++)
	{
		una_pagina = buscar_pagina_por_id(pagina);
		una_pagina->LRU = contador_LRU++;

		if (i == 0)
		{
			if (tamanio_pagina_paginacion - offset > tamanio_pcb - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_pcb - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion - offset;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio + offset, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		else
		{
			if (tamanio_pagina_paginacion > tamanio_pcb - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_pcb - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		if (i + 1 != cantidad_paginas)
		{
			pagina = siguiente_pagina(pagina, pcb_adm);
			un_frame = buscar_frame_por_pagina(pagina);
		}
	}

	un_PCB = buffer;

	return un_PCB;
}

char *buscar_tareas(int patota_id)
{
	PCB_adm *pcb_adm;
	t_list_iterator *iterador_patotas = list_iterator_create(tabla_pcbs);

	while (list_iterator_has_next(iterador_patotas))
	{
		pcb_adm = list_iterator_next(iterador_patotas);

		if (pcb_adm->PID == patota_id)
		{
			break;
		}
	}

	list_iterator_destroy(iterador_patotas);

	int cantidad_paginas = pcb_adm->cantidad_paginas_tareas;
	int pagina = pcb_adm->pagina_inicio_tareas;
	int offset = pcb_adm->offset_tareas;
	char *tareas;
	int tamanio_recuperado = 0;
	int tamanio_a_recuperar = 0;
	int tamanio_tareas = pcb_adm->tamanio_tareas;
	void *buffer = malloc(tamanio_tareas);
	int limite_buffer = 0;

	Frame *un_frame = buscar_frame_por_pagina(pagina);
	Pagina *una_pagina;

	for (int i = 0; i < cantidad_paginas; i++)
	{
		una_pagina = buscar_pagina_por_id(pagina);
		una_pagina->LRU = contador_LRU++;
		if (i == 0)
		{
			if (tamanio_pagina_paginacion - offset > tamanio_tareas - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_tareas - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion - offset;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio + offset, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		else
		{
			if (tamanio_pagina_paginacion > tamanio_tareas - tamanio_recuperado)
			{
				tamanio_a_recuperar = tamanio_tareas - tamanio_recuperado;
				tamanio_recuperado += tamanio_a_recuperar;
			}
			else
			{
				tamanio_a_recuperar = tamanio_pagina_paginacion;
				tamanio_recuperado += tamanio_a_recuperar;
			}

			memcpy(buffer + limite_buffer, un_frame->dir_inicio, tamanio_a_recuperar);

			limite_buffer = tamanio_recuperado;
		}
		if (i + 1 != cantidad_paginas)
		{
			pagina = siguiente_pagina(pagina, pcb_adm);
			un_frame = buscar_frame_por_pagina(pagina);
		}
	}

	tareas = buffer;

	return tareas;
}

//Mueve un tripulante a una dirección destino
int mover_tripulante_en_x_paginacion(int tripulante, int posicion_x_final, bool mapa)
{
	TCB *miTCB = buscar_tcb_por_id(tripulante);
	PCB_adm *pcb_adm = buscar_patota_tripulante(tripulante);

	miTCB->pos_X = posicion_x_final;

	guardar_tripulante(miTCB, pcb_adm);

	if (mapa)
	{
		char identificador = buscar_tripulante_grilla(tripulante);

		if (identificador == '-')
		{
			return -1;
		}

		mover_tripulante_grilla(identificador, 1, 0);
	}
}

int mover_tripulante_en_y_paginacion(int tripulante, int posicion_y_final, bool mapa)
{
	TCB *miTCB = buscar_tcb_por_id(tripulante);
	PCB_adm *pcb_adm = buscar_patota_tripulante(tripulante);

	miTCB->pos_y = posicion_y_final;

	guardar_tripulante(miTCB, pcb_adm);

	if (mapa)
	{
		char identificador = buscar_tripulante_grilla(tripulante);

		if (identificador == '-')
		{
			return -1;
		}

		mover_tripulante_grilla(identificador, 0, 1);
	}
}

TCB *iniciar_tripulante_paginacion(int patota_id, bool mapa)
{
	PCB_adm *pcb_adm = buscar_patota_adm(patota_id);
	TCB *un_tcb = buscar_tcb(pcb_adm, pcb_adm->tripulantes_iniciados);

	un_tcb->estado = 'R';

	guardar_tripulante(un_tcb, pcb_adm);

	pcb_adm->tripulantes_iniciados++;

	if (mapa)
	{
		crear_personaje_grilla(un_tcb->TID, un_tcb->pos_X, un_tcb->pos_y);
	}

	return un_tcb;
}

Pagina *buscar_pagina_por_id(int id_pagina)

{
	t_list_iterator *iterador_pcbs = list_iterator_create(tabla_pcbs);
	t_list_iterator *iterador_paginas;
	PCB_adm *pcb_adm;
	Pagina *una_pagina;
	bool encontre_pagina = false;

	while (list_iterator_has_next(iterador_pcbs))
	{
		pcb_adm = list_iterator_next(iterador_pcbs);

		iterador_paginas = list_iterator_create(pcb_adm->tabla_paginas);

		while (list_iterator_has_next(iterador_paginas))
		{

			una_pagina = list_iterator_next(iterador_paginas);

			if (una_pagina->id_pagina == id_pagina)
			{
				encontre_pagina = true;
				break;
			}
		}

		list_iterator_destroy(iterador_paginas);
		if (encontre_pagina)
		{
			break;
		}
	}

	list_iterator_destroy(iterador_pcbs);
	return una_pagina;
}

int iniciar_patota_paginacion(int cantidad_tripulantes, char *tareas, char *puntos)
{
	int tamanio_alocado = 0;
	Pagina *una_pagina;
	PCB_adm *pcb_adm = malloc(sizeof(PCB_adm));
	pcb_adm->PID = contador_patotas++;
	pcb_adm->tabla_paginas = list_create();
	pcb_adm->tabla_TCB_admin = list_create();
	pcb_adm->cantidad_paginas_pcb = 0;
	pcb_adm->cantidad_paginas_tareas = 0;
	pcb_adm->tripulantes_iniciados = 0;
	pcb_adm->tamanio_tareas = strlen(tareas);

	int tamanio_pcb = 8;
	int tamanio_tareas = strlen(tareas);
	int tamanio_tcb = 21;

	void *pagina = calloc(1, tamanio_pagina_paginacion);
	PCB *un_pcb = malloc(sizeof(PCB));
	TCB *un_tcb;
	TCB_adm *un_tcb_adm;

	Frame *un_frame_libre;
	int offset;
	char **lista_puntos;

	bool ubique_tareas = false;
	bool liberar_pagina = false;
	bool ubique_tripulante = false;

	char *contenido;

	char *un_punto;
	char *aux;

	un_pcb->PID = pcb_adm->PID;

	un_frame_libre = buscar_frame_libre();
	una_pagina = malloc(sizeof(Pagina));
	una_pagina->en_memoria = true;
	una_pagina->clock = -1;
	una_pagina->id_frame = un_frame_libre->id_frame;
	una_pagina->estado = "OCUPADO";
	pcb_adm->cantidad_paginas_pcb++;
	una_pagina->contenido = list_create();
	contenido = string_new();
	aux = string_itoa(un_pcb->PID);
	string_append(&contenido, "P");
	string_append(&contenido, aux);
	list_add(una_pagina->contenido, contenido);
	una_pagina->id_pagina = contador_paginas++;
	pcb_adm->pagina_inicio_pcb = una_pagina->id_pagina;

	list_add(tabla_pcbs, pcb_adm);
	list_add(pcb_adm->tabla_paginas, una_pagina);

	if ((tamanio_pcb) <= tamanio_pagina_paginacion)
	{
		if ((tamanio_pcb) == tamanio_pagina_paginacion)
		{
			offset = 0;
			liberar_pagina = true;
		}
		else
		{
			offset = tamanio_pagina_paginacion - (tamanio_pagina_paginacion - (tamanio_pcb));
		}
		memcpy(pagina, (void *)un_pcb, tamanio_pcb);
		tamanio_alocado = tamanio_pcb;
	}
	else
	{
		memcpy(pagina, (void *)un_pcb, tamanio_pagina_paginacion);
		liberar_pagina = true;
		tamanio_alocado += tamanio_pagina_paginacion;
	}

	guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
	if (liberar_pagina)
	{
		free(pagina);
		pagina = calloc(1, tamanio_pagina_paginacion);
		liberar_pagina = false;
	}

	//Cargo la patota
	while (tamanio_alocado < tamanio_pcb)
	{
		//CUIDADOOOO ESTA PRIMER APARTE VA EN UN IF AFUEREA

		un_frame_libre = buscar_frame_libre();
		una_pagina = malloc(sizeof(Pagina));
		una_pagina->en_memoria = true;
		una_pagina->clock = -1;
		una_pagina->id_frame = un_frame_libre->id_frame;
		una_pagina->estado = "OCUPADO";
		pcb_adm->cantidad_paginas_pcb++;
		una_pagina->contenido = list_create();
		contenido = string_new();
		aux = string_itoa(un_pcb->PID);
		string_append(&contenido, "P");
		string_append(&contenido, aux);
		list_add(una_pagina->contenido, contenido);
		una_pagina->id_pagina = contador_paginas++;
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
				offset = tamanio_pagina_paginacion - (tamanio_pagina_paginacion - (tamanio_pcb - tamanio_alocado));
			}
			memcpy(pagina, (void *)un_pcb + tamanio_alocado, tamanio_pcb - tamanio_alocado);
			tamanio_alocado = tamanio_pcb;
		}
		else
		{
			memcpy(pagina, (void *)un_pcb + tamanio_alocado, tamanio_pagina_paginacion);
			tamanio_alocado += tamanio_pagina_paginacion;
			liberar_pagina = true;
		}

		guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
		if (liberar_pagina)
		{
			free(pagina);
			pagina = calloc(1, tamanio_pagina_paginacion);
			liberar_pagina = false;
		}
	}

	liberar_pagina = false;
	tamanio_alocado = 0;
	pcb_adm->offset_tareas = offset;

	char *aux6 = MEMORIA + 8;

	//Cargo las tareas

	if (offset > 0)
	{
		pcb_adm->pagina_inicio_tareas = una_pagina->id_pagina;

		if ((tamanio_tareas) <= (tamanio_pagina_paginacion - offset))
		{
			memcpy(pagina + offset, (void *)tareas + tamanio_alocado, tamanio_tareas);
			if ((tamanio_tareas) == (tamanio_pagina_paginacion - offset))
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
			tamanio_alocado += tamanio_pagina_paginacion - offset;
			memcpy(pagina + offset, (void *)tareas, tamanio_pagina_paginacion - offset);

			pcb_adm->cantidad_paginas_tareas++;
		}

		guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
		contenido = string_new();
		aux = string_itoa(un_pcb->PID);
		string_append(&contenido, "Tar");
		string_append(&contenido, aux);
		list_add(una_pagina->contenido, contenido);

		if (liberar_pagina)
		{
			free(pagina);
			pagina = calloc(1, tamanio_pagina_paginacion);
			liberar_pagina = false;
		}
	}

	if (!ubique_tareas)
	{

		while (tamanio_alocado < tamanio_tareas)
		{

			un_frame_libre = buscar_frame_libre();
			una_pagina = malloc(sizeof(Pagina));
			una_pagina->en_memoria = true;
			una_pagina->clock = -1;
			una_pagina->id_pagina = contador_paginas++;
			una_pagina->id_frame = un_frame_libre->id_frame;
			una_pagina->estado = "OCUPADO";
			una_pagina->contenido = list_create();
			contenido = string_new();
			aux = string_itoa(un_pcb->PID);
			string_append(&contenido, "Tar");
			string_append(&contenido, aux);
			list_add(una_pagina->contenido, contenido);
			list_add(pcb_adm->tabla_paginas, una_pagina);
			pcb_adm->cantidad_paginas_tareas++;
			if (tamanio_alocado == 0)
			{
				pcb_adm->pagina_inicio_tareas = una_pagina->id_pagina;
			}

			if ((tamanio_tareas - tamanio_alocado) <= tamanio_pagina_paginacion)
			{
				if ((tamanio_tareas - tamanio_alocado) == tamanio_pagina_paginacion)
				{
					offset = 0;
					liberar_pagina = true;
				}
				else
				{
					offset = tamanio_tareas - tamanio_alocado;
				}
				memcpy(pagina, (void *)tareas + tamanio_alocado, tamanio_tareas - tamanio_alocado);
				tamanio_alocado = tamanio_tareas;
			}
			else
			{
				memcpy(pagina, (void *)tareas + tamanio_alocado, tamanio_pagina_paginacion);
				tamanio_alocado += tamanio_pagina_paginacion;
				liberar_pagina = true;
			}

			guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
			if (liberar_pagina)
			{
				free(pagina);
				pagina = calloc(1, tamanio_pagina_paginacion);
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
		un_tcb_adm->TID = contador_tripulantes++;
		un_tcb_adm->cantidad_paginas = 0;
		un_tcb_adm->offset = 0;
		list_add(pcb_adm->tabla_TCB_admin, un_tcb_adm);

		un_tcb = malloc(sizeof(TCB));
		un_tcb->estado = 'N';
		un_tcb->TID = un_tcb_adm->TID;
		un_tcb->PCB = pcb_adm->pagina_inicio_pcb;

		un_punto = obtener_punto_string(puntos, i);
		lista_puntos = string_split(un_punto, "|");
		un_tcb->pos_X = atoi(lista_puntos[0]);
		un_tcb->pos_y = atoi(lista_puntos[1]);
		free(lista_puntos);

		un_tcb->proxima_instruccion = 0;

		if (offset > 0)
		{
			un_tcb_adm->pagina_inicio = una_pagina->id_pagina;
			un_tcb_adm->offset = offset;
			un_tcb_adm->cantidad_paginas++;

			if ((tamanio_tcb) <= (tamanio_pagina_paginacion - offset))
			{
				memcpy(pagina + offset, (void *)un_tcb + tamanio_alocado, tamanio_tcb);
				if ((tamanio_tcb) == (tamanio_pagina_paginacion - offset))
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
				memcpy(pagina + offset, (void *)un_tcb, tamanio_pagina_paginacion - offset);
				tamanio_alocado += tamanio_pagina_paginacion - offset;
				liberar_pagina = true;
			}

			guardar_pagina_en_frame(una_pagina->id_frame, pagina, una_pagina->id_pagina);
			contenido = string_new();
			aux = string_itoa(un_tcb_adm->TID);
			string_append(&contenido, "T");
			string_append(&contenido, aux);
			list_add(una_pagina->contenido, contenido);

			if (liberar_pagina)
			{
				free(pagina);
				pagina = calloc(1, tamanio_pagina_paginacion);
				liberar_pagina = false;
			}
		}

		if (!ubique_tripulante)
		{

			while (tamanio_alocado < tamanio_tcb)
			{

				un_frame_libre = buscar_frame_libre();
				una_pagina = malloc(sizeof(Pagina));
				una_pagina->en_memoria = true;
				una_pagina->clock = -1;
				una_pagina->id_pagina = contador_paginas++;
				una_pagina->id_frame = un_frame_libre->id_frame;
				una_pagina->estado = "OCUPADO";
				una_pagina->contenido = list_create();
				contenido = string_new();
				aux = string_itoa(un_tcb_adm->TID);
				string_append(&contenido, "T");
				string_append(&contenido, aux);
				list_add(una_pagina->contenido, contenido);
				list_add(pcb_adm->tabla_paginas, una_pagina);

				if (tamanio_alocado == 0)
				{
					un_tcb_adm->pagina_inicio = una_pagina->id_pagina;
				}
				un_tcb_adm->cantidad_paginas++;

				if ((tamanio_tcb - tamanio_alocado) <= tamanio_pagina_paginacion)
				{
					if ((tamanio_tcb - tamanio_alocado) == tamanio_pagina_paginacion)
					{
						offset = 0;
						liberar_pagina = true;
					}
					else
					{
						offset = tamanio_tcb - tamanio_alocado;
					}
					memcpy(pagina, (void *)un_tcb + tamanio_alocado, tamanio_tcb - tamanio_alocado);
					tamanio_alocado = tamanio_tcb;
				}
				else
				{
					memcpy(pagina, (void *)un_tcb + tamanio_alocado, tamanio_pagina_paginacion);
					tamanio_alocado += tamanio_pagina_paginacion;
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
		ubique_tripulante = false;
	}

	return pcb_adm->PID;
}

void inicializar_paginacion(int tamanio_memoria, int tamanio_pagina, int tamanio_SWAP, char *path_SWAP, char *un_algoritmo_reemplazo)
{
	contador_frames = tamanio_memoria / tamanio_pagina;
	contador_frames_libres = contador_frames;
	contador_paginas = 1;
	tamanio_pagina_paginacion = tamanio_pagina;
	contador_espacios_SWAP = tamanio_SWAP / tamanio_pagina_paginacion;
	Frame *un_frame;
	tabla_frames = list_create();
	tabla_pcbs = list_create();
	tabla_tareas = list_create();
	tabla_tcbs = list_create();
	tabla_elementos_SWAP = list_create();
	contador_LRU = 0;
	algoritmo_reemplazo = un_algoritmo_reemplazo;
	cola_FIFO_clock = queue_create();

	void *bitarray = calloc(contador_espacios_SWAP / 8, 1);

	bitmap = bitarray_create_with_mode(bitarray, contador_espacios_SWAP / 8, MSB_FIRST);

	void *pagina = malloc(tamanio_pagina_paginacion);

	SWAP = fopen(path_SWAP, "w+");

	//for (int i = 0; i < contador_espacios_SWAP; i++)
	//{
	//	fwrite(&pagina, sizeof(pagina), 1, SWAP);
	//}

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