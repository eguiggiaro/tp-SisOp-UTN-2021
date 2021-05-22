#include "segmento.h"

//Muestra el contenido de la tabla de segmentos
void mostrar_tabla_segmentos(bool mostrar_vacios)

{
	t_list_iterator* list_iterator = list_iterator_create(tabla_segmentos);
                while(list_iterator_has_next(list_iterator)) {
                    segmento* segmento_aux = list_iterator_next(list_iterator);
                    
					//Imprime solo segmentos libres o no, según parámetro de entrada
					if (mostrar_vacios || (segmento_aux->id) >= 0)
						imprimir_segmento(segmento_aux);
                }
	list_iterator_destroy(list_iterator);
}

//Imprime el segmento
//TO DO no es muy amigable el resultado MEJORAR!

void imprimir_segmento(segmento* segmento_a_imprimir)

{
		miLogInfo("Id del segmento: %d\n",segmento_a_imprimir->id);
		miLogInfo("Direccion de inicio %d\n",segmento_a_imprimir->dir_inicio);
		miLogInfo("Desplazamiento %d\n",segmento_a_imprimir->desplazamiento);	
		miLogInfo("El estado es %s\n", segmento_a_imprimir->estado);
}

//Imprime el segmento, no es muy amigable, MEJORAR!
int reservar_memoria_segmentacion_ff(int bytes)
{
	int index = 0;
	bool encontre_segmento = false;
	t_list_iterator* list_iterator = list_iterator_create(tabla_segmentos);
                segmento* segmento_a_ocupar;
				segmento* segmento_nuevo;				
				while(list_iterator_has_next(list_iterator)) {
                    segmento_a_ocupar = list_iterator_next(list_iterator);

                    //Si encuentro un segmento libre donde cabe el espacio a ocupar
					if ((strcmp(segmento_a_ocupar->estado,"LIBRE") == 0) && (segmento_a_ocupar->desplazamiento >= bytes))
					{
                            encontre_segmento = true;

                            //Seteo el nuevo segmento
							segmento_nuevo = malloc(sizeof(segmento));
							segmento_nuevo->id = contador_segmentos++;
							segmento_nuevo->dir_inicio =  segmento_a_ocupar->dir_inicio;
							segmento_nuevo->desplazamiento = bytes;
							segmento_nuevo->estado = "OCUPADO";
							
                            //Reduzco el segmento libre
							segmento_a_ocupar->dir_inicio =  (segmento_nuevo->dir_inicio + (segmento_nuevo->desplazamiento));
							segmento_a_ocupar->desplazamiento = segmento_a_ocupar->desplazamiento - bytes;
							
                            //Agrego el nuevo segmento a la tabla
							list_add_in_index(tabla_segmentos,index,segmento_nuevo);
							break;
					}
					index++;

                }
	list_iterator_destroy(list_iterator);

	if (encontre_segmento)
	{
		return segmento_nuevo->dir_inicio;
	} else
	{
		return 99; //ERROR
	}
}




int alta_patota_segmentacion(PCB* unPCB)
{
	 list_add_in_index(tabla_segmentos_pcb,unPCB->PID,unPCB);
}

uint32_t buscar_patota_segmentacion(int PCB_ID)
{
	PCB* unPCB;
	
	if (PCB_ID > list_size(tabla_segmentos_pcb) -1)
	{
		return 99;
	} else {
		unPCB = list_get(tabla_segmentos_pcb, PCB_ID); 
	}
	return unPCB;
}

void alta_tripulante_segmentacion(TCB* unTCB)
{
	 list_add_in_index(tabla_segmentos_tcb,unTCB->TID,unTCB);
}

uint32_t buscar_tripulante_segmentacion(int TCB_ID)
{
	TCB* unTCB;
	
	if (TCB_ID > list_size(tabla_segmentos_tcb) -1)
	{
		return 99;
	} else {
		unTCB = list_get(tabla_segmentos_tcb, TCB_ID); 
	}
	return unTCB;
}

int segmentacion_nuevo_pcb(PCB* unPCB)
{
	int index = 0;
	int encontre_segmento = -1;
	t_list_iterator* list_iterator = list_iterator_create(tabla_segmentos);
                segmento* segmento_a_ocupar;
				
				while(list_iterator_has_next(list_iterator)) {
                    segmento_a_ocupar = list_iterator_next(list_iterator);

                    //Si encuentro un segmento libre donde cabe el espacio a ocupar
					if ((strcmp(segmento_a_ocupar->estado,"LIBRE") == 0) && (segmento_a_ocupar->desplazamiento >= sizeof(PCB*)))
					{
                            encontre_segmento = 0;

                            //Seteo el nuevo segmento
							segmento* segmento_nuevo = malloc(sizeof(segmento));
							segmento_nuevo->id = contador_segmentos++;
							segmento_nuevo->dir_inicio =  segmento_a_ocupar->dir_inicio;
							segmento_nuevo->desplazamiento = sizeof(PCB);
							segmento_nuevo->estado = "OCUPADO";
							
                            //Reduzco el segmento libre
							segmento_a_ocupar->dir_inicio =  (segmento_nuevo->dir_inicio + (segmento_nuevo->desplazamiento));
							segmento_a_ocupar->desplazamiento = segmento_a_ocupar->desplazamiento - (sizeof(PCB));
							
                            //Copio el TCB a memoria
							memcpy(segmento_nuevo->dir_inicio, unPCB, sizeof(PCB));

                            //Agrego el nuevo segmento a la tabla
							list_add_in_index(tabla_segmentos,index,segmento_nuevo);
							break;
					}
					index++;

                }
	list_iterator_destroy(list_iterator);

	return encontre_segmento;
}

//Crea la tabla de segmentos y el primer segmento vacio
void inicializar_segmentacion(int tamanio_memoria) 
{
    contador_segmentos = 0;
	tabla_segmentos = list_create();
	tabla_segmentos_pcb = list_create();
	tabla_segmentos_tareas = list_create();
	tabla_segmentos_tcb = list_create();

    //Creo un segmento vacío del tamanio de la memoria
	segmento* segmento_aux = malloc(sizeof(segmento));
	segmento_aux->id = -1;
	segmento_aux->dir_inicio =  MEMORIA;
	segmento_aux->desplazamiento = tamanio_memoria;
	segmento_aux->estado = "LIBRE";

	//Agrego el segmento vacío a la tabla de segmentos
	list_add(tabla_segmentos, segmento_aux);
}

//Finaliza la segmentación
void finalizar_segmentacion()
{
    list_destroy(tabla_segmentos);
	list_destroy(tabla_segmentos_pcb);
	list_destroy(tabla_segmentos_tareas);
	list_destroy(tabla_segmentos_tcb);
}