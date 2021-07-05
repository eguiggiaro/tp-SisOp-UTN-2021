#include "segmento.h"

void archivo_segmentacion(bool mostrar_vacios)

{
	pthread_mutex_lock(&mutex_dump);

	char* nombre_archivo = string_new();
	string_append(&nombre_archivo, "dmp/Dump_");
	string_append(&nombre_archivo, temporal_get_string_time("%d_%m_%y_%H:%M:%S"));
	string_append(&nombre_archivo, ".dmp");

	FILE* archivoDump = fopen(nombre_archivo, "w+");

	dump_memoria_segmentos_archivo(mostrar_vacios, archivoDump);
	dump_memoria_contenido_segmentacion_archivo(archivoDump);	
	
	fclose(archivoDump);
	fflush(stdout); 
	pthread_mutex_unlock(&mutex_dump);

}

void dump_memoria_segmentos_archivo(bool mostrar_vacios, FILE* archivoDump)
{
	
	fprintf(archivoDump,"--------------------------------------------------------------------\n");
	fprintf(archivoDump,"Detalle segmentos en memoria\n");
	fprintf(archivoDump,"--------------------------------------------------------------------\n");
	fprintf(archivoDump,"Dir Inicio\tId segmento\tDesplazamiento\tEstado\n");
	
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	while (list_iterator_has_next(list_iterator))
	{
		Segmento *segmento_aux = list_iterator_next(list_iterator);

		//Imprime solo segmentos libres o no, según parámetro de entrada
		if (mostrar_vacios || (segmento_aux->id) >= 0)
			imprimir_segmento_archivo(segmento_aux, archivoDump);
	}

	fprintf(archivoDump,"--------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator);
}



void dump_memoria_contenido_segmentacion_archivo(FILE* archivoDump)
{

	fprintf(archivoDump,"Detalle patotas en memoria\n");
	fprintf(archivoDump,"--------------------------------------------------------------------\n");
	fprintf(archivoDump,"Patota Id\t# Segmento\t  Tipo\t   Inicio\tTamaño\n");

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

		fprintf(archivoDump,"%9d\t%10d\t%6s\t%p\t%5db\n", patota, segmento, tipo,pcb,tamanio);

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

				fprintf(archivoDump,"%9d\t%10d\t%4s\t%p\t%5db\n", patota, segmento, tipo,tareas,tamanio);
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

						fprintf(archivoDump,"%9d\t%10d\t%4s\t%p\t%5db\n", patota, segmento, tipo,tcb,tamanio);
					}
			}
		list_iterator_destroy(list_iterator_tcb);

	}

	fprintf(archivoDump,"--------------------------------------------------------------------\n");
	list_iterator_destroy(list_iterator_pcb);
}

void imprimir_segmento_archivo(Segmento *segmento_a_imprimir, FILE* archivoDump)

{
	fprintf(archivoDump,"%d\t%d\t\t%d\t\t%s\n", segmento_a_imprimir->dir_inicio, segmento_a_imprimir->id, segmento_a_imprimir->desplazamiento, segmento_a_imprimir->estado);
}








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
	list_sort(tabla_segmentos, segmentos_orden);

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

u_int32_t reservar_memoria_segmentacion_bf(int bytes)
{
	int index = 0;
	bool encontre_segmento = false;
	t_list_iterator *list_iterator = list_iterator_create(tabla_segmentos);
	Segmento *segmento_a_ocupar;
	Segmento *segmento_auxiliar;
	Segmento *segmento_nuevo;
	u_int32_t posicion_reservada;
	int minimo_desplazamiento = tamanio_memoria_segmentacion +1;


	while (list_iterator_has_next(list_iterator))
	{
		segmento_auxiliar = list_iterator_next(list_iterator);

		//Si encuentro un segmento libre donde cabe el espacio a ocupar
		if ((strcmp(segmento_auxiliar->estado, "LIBRE") == 0) && (segmento_auxiliar->desplazamiento >= bytes) && segmento_auxiliar->desplazamiento < minimo_desplazamiento)
		{
			minimo_desplazamiento = segmento_auxiliar->desplazamiento;
			segmento_a_ocupar = segmento_auxiliar;
			encontre_segmento = true;
		}
		index++;

	}
	
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


char *buscar_posicion_tripulante_segmentacion(int tripulante_id)
{

	u_int32_t posicion_memoria = buscar_tripulante(tripulante_id);

	if (posicion_memoria == 99)
	{
		return "";
	}

	TCB *unTCB = posicion_memoria;
	char *posicion_tripulante;

	posicion_tripulante = string_itoa(unTCB->pos_X);
	string_append(&posicion_tripulante, "|");
	string_append(&posicion_tripulante, string_itoa(unTCB->pos_y));

	return posicion_tripulante;
}

char *proxima_tarea_tripulante_segmentacion(int tripulante_id)
{

	u_int32_t posicion_memoria = buscar_tripulante(tripulante_id);

	if (posicion_memoria == 99)
	{
		return "";
	}

	TCB *unTCB = posicion_memoria;

	char *proxima_tarea = unTCB->proxima_instruccion;

	if (proxima_tarea[0] == '$')
	{
		return proxima_tarea;
	}

	int index = 0;
	char *string_tarea = string_new();
	char caracter;

	while (1)
	{ //string con todas las posiciones
		caracter = proxima_tarea[index];
		if (caracter == '$')
		{
			break;
		}

		if (caracter == '|')
		{
			string_append_with_format(&string_tarea, "%c", caracter);
			index++;
			break;
		}
		else
		{
			string_append_with_format(&string_tarea, "%c", caracter);
			index++;
		}
	}

	unTCB->proxima_instruccion = unTCB->proxima_instruccion + index;

	return string_tarea;
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

int compactar_segmentacion()
{
	t_list_iterator	*list_iterator_segmentos = list_iterator_create(tabla_segmentos);
	Segmento* segmento1;
	Segmento* segmento2;
	contador_segmentos = 0;
	contador_segmentos_libres = -1;
	int contador_lista = 0;
	bool borrar_segmento = false;

	if (list_iterator_has_next(list_iterator_segmentos)) {
		segmento1 = list_iterator_next(list_iterator_segmentos);
	}

	while (list_iterator_has_next(list_iterator_segmentos))
		{
			segmento2 = list_iterator_next(list_iterator_segmentos);

			if (strcmp(segmento1->estado,"LIBRE") == 0 && strcmp(segmento2->estado,"OCUPADO") == 0) 
			{
				compactacion_mover_segmentos(segmento1, segmento2, contador_segmentos);
				contador_segmentos++;
				segmento1 = segmento2;

			} else {
				if (strcmp(segmento1->estado,"LIBRE") == 0 && strcmp(segmento2->estado,"LIBRE") == 0) 
				{
					compactacion_fusionar_segmentos(segmento1, segmento2);
					free(segmento2);
					list_iterator_remove(list_iterator_segmentos);
				} else {
							compactacion_renumerar_lleno(segmento1, contador_segmentos);
							contador_segmentos++;
							segmento1 = segmento2;
				}
				}
			contador_lista++;
			dump_memoria_segmentacion(true);

		}

		if (strcmp(segmento1->estado,"OCUPADO") == 0) 
		{
			compactacion_renumerar_lleno(segmento1, contador_segmentos);
			contador_segmentos++;
		} else {
			compactacion_renumerar_vacio(segmento1, contador_segmentos_libres);
			contador_segmentos_libres--;
		}

		list_iterator_destroy(list_iterator_segmentos);

		dump_memoria_segmentacion(true);
}


int fusion_segmentos_libres()
{
	t_list_iterator	*list_iterator_segmentos = list_iterator_create(tabla_segmentos);
	Segmento* segmento1;
	Segmento* segmento2;

	if (list_iterator_has_next(list_iterator_segmentos)) {
		segmento1 = list_iterator_next(list_iterator_segmentos);
	}

	while (list_iterator_has_next(list_iterator_segmentos))
		{
			segmento2 = list_iterator_next(list_iterator_segmentos);

			if (strcmp(segmento1->estado,"LIBRE") == 0 && strcmp(segmento2->estado,"LIBRE") == 0) 
			{
					compactacion_fusionar_segmentos(segmento1, segmento2);
					free(segmento2);
					list_iterator_remove(list_iterator_segmentos);
			} else {
				segmento1 = segmento2;
			}
		}

		list_iterator_destroy(list_iterator_segmentos);

		dump_memoria_segmentacion(true);
}


void compactacion_renumerar_lleno(Segmento* segmento1, int segmento_lleno)
{
	t_list_iterator *list_iterator_pcb;
	t_list_iterator *list_iterator_tcb;
	t_list_iterator *list_iterator_tareas;
	PCB_adm* pcb_adm;
	TCB_adm* tcb_adm;
	Tarea_adm *tarea_adm;
	bool encontre_elemento = false;

	list_iterator_pcb = list_iterator_create(tabla_segmentos_pcb);

	while (list_iterator_has_next(list_iterator_pcb))
	{
		pcb_adm = list_iterator_next(list_iterator_pcb);

		if (pcb_adm->segmento_nro == segmento1->id) {
			pcb_adm->segmento_nro = segmento_lleno;
			encontre_elemento = true;
			break;
		}
	}

	list_iterator_destroy(list_iterator_pcb);
	list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);


	if (!encontre_elemento) 
	{
		while (list_iterator_has_next(list_iterator_tcb))
		{
			tcb_adm = list_iterator_next(list_iterator_tcb);

			if (tcb_adm->segmento_nro == segmento1->id) {
				tcb_adm->segmento_nro = segmento_lleno;
				encontre_elemento = true;
				break;
			}
		}
	}

	list_iterator_destroy(list_iterator_tcb);
	list_iterator_tareas = list_iterator_create(tabla_segmentos_tareas);


	if (!encontre_elemento) 
	{
		while (list_iterator_has_next(list_iterator_tareas))
		{
			tarea_adm = list_iterator_next(list_iterator_tareas);

			if (tarea_adm->segmento_nro == segmento1->id) {
				tarea_adm->segmento_nro = segmento_lleno;
				encontre_elemento = true;
				break;
			}
		}
	}

	list_iterator_destroy(list_iterator_tareas);

	segmento1->id = segmento_lleno;
}


void compactacion_renumerar_vacio(Segmento* segmento1, int segmento_vacio)
{
	segmento1->id = segmento_vacio;
}

void compactacion_fusionar_segmentos(Segmento* segmento1, Segmento* segmento2)
{
	segmento1->desplazamiento = (segmento1->desplazamiento) + (segmento2->desplazamiento);
}


void compactacion_mover_segmentos(Segmento* segmento1, Segmento* segmento2, int segmento_lleno)
{

	t_list_iterator	*list_iterator_tcb;
	t_list_iterator	*list_iterator_pcb;
	t_list_iterator	*list_iterator_tareas;

	int desplazamiento_final_1;
	int desplazamiento_final_2;

	TCB_adm* tcb_adm2;
	PCB_adm* pcb_adm2;
	Tarea_adm* tarea_adm2;
	bool encontre_elemento = false;
	char tipo_encontrado = 'N';

	int segmento_pcb;
	int segmento_tcb;
	int segmento_tareas;

	TCB* tcb_auxiliar;
	PCB* pcb_auxiliar;
	char* tareas_auxiliar;

	//Borrar!
	TCB* tcb_TEST;
	PCB* pcb_TEST;
	char* tareas_TEST;

	Segmento* segmento_auxiliar;

	//Segmento2 (que es el lleno)
	list_iterator_pcb = list_iterator_create(tabla_segmentos_pcb);

	while (list_iterator_has_next(list_iterator_pcb))
	{
		pcb_adm2 = list_iterator_next(list_iterator_pcb);

		if (pcb_adm2->segmento_nro == segmento2->id) {
			encontre_elemento = true;
			tipo_encontrado = 'P';
			break;
		}
	}

	list_iterator_destroy(list_iterator_pcb);
	list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);


	if (!encontre_elemento) 
	{
		while (list_iterator_has_next(list_iterator_tcb))
		{
			tcb_adm2 = list_iterator_next(list_iterator_tcb);

			if (tcb_adm2->segmento_nro == segmento2->id) {
				encontre_elemento = true;
				tipo_encontrado = 'T';
				break;
			}
		}
	}

	list_iterator_destroy(list_iterator_tcb);
	list_iterator_tareas = list_iterator_create(tabla_segmentos_tareas);


	if (!encontre_elemento) 
	{
		while (list_iterator_has_next(list_iterator_tareas))
		{
			tarea_adm2 = list_iterator_next(list_iterator_tareas);

			if (tarea_adm2->segmento_nro == segmento2->id) {
				encontre_elemento = true;
				tipo_encontrado = 'K';
				break;
			}
		}
	}

	list_iterator_destroy(list_iterator_tareas);


	if (tipo_encontrado == 'P')
	{
		pcb_auxiliar = segmento2->dir_inicio;
		
		list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);
		

		list_iterator_tareas = list_iterator_create(tabla_segmentos_tareas);
		while (list_iterator_has_next(list_iterator_tareas))
		{
			tarea_adm2 = list_iterator_next(list_iterator_tareas);

			if (tarea_adm2->PID == pcb_auxiliar->PID) {
				segmento_auxiliar = buscar_segmento_por_id(tarea_adm2->segmento_nro);
				tareas_auxiliar = segmento_auxiliar->dir_inicio;
				break;
			}
		}
		list_iterator_destroy(list_iterator_tareas);		
		
		//muevo el elemento
		desplazamiento_final_1 = segmento2->desplazamiento;
		desplazamiento_final_2 = segmento1->desplazamiento;


		memcpy(segmento1->dir_inicio, pcb_auxiliar, segmento2->desplazamiento);
	
		//Cambio los segmentos

		segmento2->id = segmento1->id;
		segmento2->desplazamiento = desplazamiento_final_2;
		segmento2->estado = "LIBRE";
		segmento2->dir_inicio = segmento1->dir_inicio + desplazamiento_final_1 + 1;

		segmento1->desplazamiento = desplazamiento_final_1;
		segmento1->estado = "OCUPADO";
		segmento1->id = segmento_lleno;


		
		//Cambio estructura administrativa
		pcb_adm2->segmento_nro = segmento_lleno;
		
		while (list_iterator_has_next(list_iterator_tcb))
		{
			tcb_adm2 = list_iterator_next(list_iterator_tcb);

			if (tcb_adm2->PID == pcb_auxiliar->PID) {
				segmento_auxiliar = buscar_segmento_por_id(tcb_adm2->segmento_nro);
				segmento_auxiliar = segmento_auxiliar->dir_inicio;
				//Cambio estructuras memoria
				tcb_auxiliar->PCB = segmento1->dir_inicio;
			}
		}
		list_iterator_destroy(list_iterator_tcb);


		pcb_TEST = segmento1->dir_inicio;

	}

	if (tipo_encontrado == 'T')
	{
		tcb_auxiliar = segmento2->dir_inicio;
		
		//muevo el elemento
		desplazamiento_final_1 = segmento2->desplazamiento;
		desplazamiento_final_2 = segmento1->desplazamiento;

		memcpy(segmento1->dir_inicio, tcb_auxiliar, segmento2->desplazamiento);
	
		//Cambio los segmentos
		segmento2->id = segmento1->id;
		segmento2->desplazamiento = desplazamiento_final_2;
		segmento2->estado = "LIBRE";
		segmento2->dir_inicio = segmento1->dir_inicio + desplazamiento_final_1 + 1;

		segmento1->desplazamiento = desplazamiento_final_1;
		segmento1->estado = "OCUPADO";
		segmento1->id = segmento_lleno;

		//Cambio estructura administrativa
		tcb_adm2->segmento_nro = segmento_lleno;

		tcb_TEST = segmento1->dir_inicio;

		
	}

	if (tipo_encontrado == 'K')
	{
		tareas_auxiliar = segmento2->dir_inicio;

		list_iterator_pcb = list_iterator_create(tabla_segmentos_pcb);
		while (list_iterator_has_next(list_iterator_pcb))
		{
			pcb_adm2 = list_iterator_next(list_iterator_pcb);

			if (pcb_adm2->PID == tarea_adm2->PID) {
				segmento_auxiliar = buscar_segmento_por_id(pcb_adm2->segmento_nro);
				pcb_auxiliar = segmento_auxiliar->dir_inicio;
				
			}
		}
		list_iterator_destroy(list_iterator_pcb);

		//muevo el elemento
		desplazamiento_final_1 = segmento2->desplazamiento;
		desplazamiento_final_2 = segmento1->desplazamiento;

		memcpy(segmento1->dir_inicio, tareas_auxiliar, segmento2->desplazamiento);
	
		//Cambio los segmentos
		segmento2->id = segmento1->id;
		segmento2->desplazamiento = desplazamiento_final_2;
		segmento2->estado = "LIBRE";
		segmento2->dir_inicio = segmento1->dir_inicio + desplazamiento_final_1 + 1;

		segmento1->desplazamiento = desplazamiento_final_1;
		segmento1->estado = "OCUPADO";
		segmento1->id = segmento_lleno;
		
		//Cambio estructura administrativa
		tarea_adm2->segmento_nro = segmento_lleno;

		//Cambio estructuras memoria
		pcb_auxiliar->Tareas = tareas_auxiliar;

		list_iterator_tcb = list_iterator_create(tabla_segmentos_tcb);
		while (list_iterator_has_next(list_iterator_tcb))
		{
			tcb_adm2 = list_iterator_next(list_iterator_tcb);

			if (tcb_adm2->PID == pcb_auxiliar->PID) {
				segmento_auxiliar = buscar_segmento_por_id(tcb_adm2->segmento_nro);
				tcb_auxiliar = segmento_auxiliar->dir_inicio;

				tcb_auxiliar->proxima_instruccion = (tcb_auxiliar->proxima_instruccion) - desplazamiento_final_1;
			}
		}
		list_iterator_destroy(list_iterator_tcb);

		tareas_TEST = segmento1->dir_inicio;


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

	fusion_segmentos_libres();

	if (elimine_tripulante)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

//Mueve un tripulante a una dirección destino
int mover_tripulante_en_x_segmentacion(int tripulante, int posicion_x_final, bool mapa)
{
	TCB *miTCB = buscar_tripulante(tripulante);

	if (miTCB == 99)
	{
		return -1;
	}

	miTCB->pos_X = posicion_x_final;
	if (mapa){
	char identificador = buscar_tripulante_grilla(tripulante);

	if (identificador == '-')
	{
		return -1;
	}

	mover_tripulante_grilla(identificador, 1,0);
	}
}

int mover_tripulante_en_y_segmentacion(int tripulante, int posicion_y_final, bool mapa)
{
	TCB *miTCB = buscar_tripulante(tripulante);

	if (miTCB == 99)
	{
		return -1;
	}

	miTCB->pos_y = posicion_y_final;

	if (mapa){
	char identificador = buscar_tripulante_grilla(tripulante);

	if (identificador == '-')
	{
		return -1;
	}

	mover_tripulante_grilla(identificador, 0,1);
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


TCB* iniciar_tripulante_segmentacion(int patota_id, bool mapa)
{
	u_int32_t posicion_memoria = buscar_tripulante_no_asignado_segmentacion(patota_id);

	if (posicion_memoria == 99)
	{
		return -1;
	}

	TCB *unTCB = posicion_memoria;
	unTCB->estado = 'R';

	if (mapa) {
		crear_personaje_grilla(unTCB->TID, unTCB->pos_X, unTCB->pos_y);
	}

	return unTCB;
}



//Crea la tabla de segmentos y el primer segmento vacio
void inicializar_segmentacion(int tamanio_memoria, char* criterio)
{
	contador_segmentos = 0;
	contador_segmentos_libres = -1;
	tabla_segmentos = list_create();
	tabla_segmentos_pcb = list_create();
	tabla_segmentos_tareas = list_create();
	tabla_segmentos_tcb = list_create();
	criterio_seleccion = criterio;
	tamanio_memoria_segmentacion = tamanio_memoria;

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
	alta_tripulante_segmentacion(miTCB, patota);
}


bool segmentos_menor (Segmento* segmento1, Segmento* segmento_mayor)
{
	return segmento1->desplazamiento <= segmento_mayor->desplazamiento;
}

bool segmentos_orden (Segmento* segmento1, Segmento* segmento_mayor)
{
	return segmento1->dir_inicio <= segmento_mayor->dir_inicio;
}




int verifica_espacio(int cantidad_tripulantes, int tareas_tamanio)
{
	t_list_iterator	*list_iterator_segmentos = list_iterator_create(tabla_segmentos);
	t_list* tabla_segmentos_libres = list_create();
	Segmento* segmento1;
	Segmento* segmento_aux;
	int contador_total_disponible = 0;
	int contador_total_requerido;
	bool ubique_pcb = false;
	bool ubique_tareas = false;
	bool ubique_tripulantes = false;
	int tripulantes_ubicados = 0;
	bool hay_lugar_total = false;
	bool hay_lugar_parcial = false;
	bool salto_de_segmento = false;

	contador_total_requerido = 8 + (21*cantidad_tripulantes) + tareas_tamanio;

	while (list_iterator_has_next(list_iterator_segmentos))
	{
			segmento1 = list_iterator_next(list_iterator_segmentos);

			if (strcmp(segmento1->estado,"LIBRE") == 0) 
			{
				segmento_aux = malloc(sizeof(Segmento));
				segmento_aux->id = segmento1->id;
				segmento_aux->desplazamiento = segmento1->desplazamiento;

				contador_total_disponible += segmento1->desplazamiento;
				if (strcmp(criterio_seleccion,"FF")==0)
				{
					list_add(tabla_segmentos_libres,segmento_aux);
				} else {
					list_add_sorted(tabla_segmentos_libres,segmento_aux, segmentos_menor);
				}
			}
	}


	t_list_iterator	*list_iterator_segmentos_libres = list_iterator_create(tabla_segmentos_libres);

	while (list_iterator_has_next(list_iterator_segmentos_libres))
	{
		segmento1 = list_iterator_next(list_iterator_segmentos_libres);

	//	if (strcmp(criterio_seleccion,"FF")==0) 
	//	{
			if (!ubique_pcb)
			{
				if(segmento1->desplazamiento>= 8)
				{
					ubique_pcb = true;
					segmento1->desplazamiento = segmento1->desplazamiento - 8;
				}
			}
			if (!ubique_tareas && ubique_pcb)
			{
				if(segmento1->desplazamiento >= tareas_tamanio)
				{
					ubique_tareas = true;
					segmento1->desplazamiento = segmento1->desplazamiento - tareas_tamanio;
				}
			}

			for (int i=0; i < cantidad_tripulantes; i++)
			{
				if (!ubique_tripulantes && ubique_pcb && ubique_tareas)
				{
					if(segmento1->desplazamiento >= 21)
					{
						segmento1->desplazamiento = segmento1->desplazamiento - 21;
						tripulantes_ubicados++;
						if (tripulantes_ubicados == cantidad_tripulantes)
						{
							ubique_tripulantes = true;
						}
					}
				}				
			}
	//	}
	}

	list_destroy(tabla_segmentos_libres);
	list_iterator_destroy(list_iterator_segmentos);
	list_iterator_destroy(list_iterator_segmentos_libres);
	free(segmento_aux);
	
		if (contador_total_disponible >= contador_total_requerido)
		{
			hay_lugar_total = true;
		}

		if (ubique_pcb && ubique_tareas && ubique_tripulantes)
		{
			hay_lugar_parcial = true;
		}

		if (hay_lugar_total)
		{
			if(hay_lugar_parcial)
			{
				return 0;
			} else {
				return -2;
			}
		} else {
			return -1;
		}
}



int iniciar_patota_segmentacion(int cantidad_tripulantes, char *tareas, char *puntos)
{

	int resultado_verificador_espacio = verifica_espacio(cantidad_tripulantes, strlen(tareas));

	if (resultado_verificador_espacio == -1)
	{
		miLogInfo("No hay más espacio en memoria \n");
		return -1;
	}

	if (resultado_verificador_espacio == -2)
	{
		miLogInfo("Hay espacio, pero es necesario compactar. Compactando.. \n");
		compactar_segmentacion();
	}

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