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
//TODO el algoritmo debería ser un parámetro o condicion, no está dejando ok el segmento libre
void nuevo_segmento_patota_first_fit_tcb(TCB* unTCB)
{
	int index = 0;
	t_list_iterator* list_iterator = list_iterator_create(tabla_segmentos);
                segmento* segmento_a_ocupar;
				
				while(list_iterator_has_next(list_iterator)) {
                    segmento_a_ocupar = list_iterator_next(list_iterator);

                    //Si encuentro un segmento libre donde cabe el espacio a ocupar
					if ((segmento_a_ocupar->estado = 'LIBRE') && (segmento_a_ocupar->desplazamiento >= sizeof(unTCB)))
					{
                            
                            //Seteo el nuevo segmento
							segmento* segmento_nuevo = malloc(sizeof(segmento));
							segmento_nuevo->id = contador_segmentos++;
							segmento_nuevo->dir_inicio =  segmento_a_ocupar;
							segmento_nuevo->desplazamiento = sizeof(unTCB);
							segmento_nuevo->estado = "OCUPADO";
							
                            //Reduzco el segmento libre
							segmento_a_ocupar->dir_inicio =  (segmento_nuevo + (segmento_nuevo->desplazamiento));
							segmento_a_ocupar->desplazamiento = segmento_a_ocupar->desplazamiento - (sizeof(unTCB));
							
                            //Copio el TCB a memoria
							memcpy(segmento_nuevo->dir_inicio, unTCB, sizeof(unTCB));

                            //Agrego el nuevo segmento a la tabla
							list_add_in_index(tabla_segmentos,index,segmento_nuevo);
							break;
					}
					index++;

                }
	list_iterator_destroy(list_iterator);

}

//Crea la tabla de segmentos y el primer segmento vacio
void inicializar_segmentacion(int tamanio_memoria) 
{
    contador_segmentos = 0;
	tabla_segmentos = list_create();

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
}