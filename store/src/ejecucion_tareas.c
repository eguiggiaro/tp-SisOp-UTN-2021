#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


int generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	char * cadenaCaracteres = generarCadenaCaracteres(metadataR->caracter_llenado, cantidadCaracteres);
	
	int size = metadataR->size;
	int block_count = metadataR->block_count;
	int posicionUltimoBloque = list_size(metadataR->blocks);
	int ultimoBloque = 0;
	
	if (posicionUltimoBloque != 0){
		posicionUltimoBloque--;	
		ultimoBloque = list_get(metadataR->blocks, posicionUltimoBloque);	
	} 
	
	t_list* listaBloquesOcupados = llenarBloque(size, block_count, ultimoBloque, cadenaCaracteres);

	free(cadenaCaracteres);
	
	if(listaBloquesOcupados == NULL){
		//list_destroy_and_destroy_elements(listaBloquesOcupados, (void*) free);
		freeMetadataRecurso(metadataR);
		return -1;
	}

	list_add_all(metadataR->blocks, listaBloquesOcupados);
	metadataR->size += cantidadCaracteres;
	metadataR->block_count += list_size(listaBloquesOcupados);

	list_destroy(listaBloquesOcupados);

	if(modificarMetadataRecurso(metadataR, recurso)){
		return -1;
	}     
	return 1;
}


t_list* llenarBloque(int size, int blockCount, int ultimoBloque, char* cadenaCaracteres){
   
    int cantidadBytesLibres = bytesLibresUltimoBloque(size, blockCount);
    int posicionEnCadena = cantidadBytesLibres;
	t_list* bloquesEscritos;

    if(cantidadBytesLibres > 0){

        char* cadenaHastaCantidad = truncarCadenaHastaCantidad(cadenaCaracteres, posicionEnCadena);
        escribirBloqueUsado(ultimoBloque, cantidadBytesLibres, cadenaHastaCantidad);

		if(string_length(cadenaHastaCantidad) == string_length(cadenaCaracteres)){
			free(cadenaHastaCantidad);
			bloquesEscritos = list_create();
			return bloquesEscritos;
		}
		free(cadenaHastaCantidad);
	}
    
	char* cadenaDesdeCantidad = truncarCadenaDesdeCantidad(cadenaCaracteres, posicionEnCadena);  
	bloquesEscritos = escribirBloquesNuevo(cadenaDesdeCantidad);

	free(cadenaDesdeCantidad);
	
    return bloquesEscritos;
    
}

char * generarCadenaCaracteres(char caracter, int cantidadCaracteres){

    /*char * cadenaDeCaracteres;
	cadenaDeCaracteres = malloc(sizeof (char) * cantidadCaracteres);
	cadenaDeCaracteres = string_repeat(caracter, cantidadCaracteres);*/
    return string_repeat(caracter, cantidadCaracteres);  
}

char cualEsMiCaracter(tipoRecurso recurso){

	char caracter;
	switch (recurso)
	{
		case OXIGENO:
			//printf( "OXIGENO\n" );
			caracter = 'O';
			break;

		case COMIDA:
			//printf( "COMIDA\n" );
			caracter = 'C';
			break;

		case BASURA:
			//printf( "BASURA\n" );
			caracter = 'B';
			break;

		default:
			printf( "Error: recurso inexistente\n" );
			break;
	}
	return caracter;
}


int bytesLibresUltimoBloque(int size, int blockCount){

	int bytesLibres = blockCount * tamanioBloque - size;

	return bytesLibres; 
} 

char * truncarCadenaDesdeCantidad(char* cadenaCaracteres, int posicionEnCadena){

	char * cadenaDesdeCantidad = string_substring_from(cadenaCaracteres, posicionEnCadena);
	return cadenaDesdeCantidad;
	
	}

char * truncarCadenaHastaCantidad(char* cadenaCaracteres, int posicionEnCadena){
	
	char * cadenaHastaCantidad = string_substring_until(cadenaCaracteres, posicionEnCadena);
	return cadenaHastaCantidad;

}

int guardarEnBitacora(char* id_tripulante, char* instruccion){
	
	pthread_mutex_lock(&mutex_guardar_en_bitacora);
	MetadataBitacora* metadata = leerMetadataBitacora(id_tripulante);

	int size = metadata->size;
	int block_count = metadata->block_count;
	int posicionUltimoBloque = list_size(metadata->blocks);
	int ultimoBloque = 0;
	
	if (posicionUltimoBloque != 0){
		posicionUltimoBloque--;
		ultimoBloque = list_get(metadata->blocks, posicionUltimoBloque);
	} 

	t_list* listaBloquesOcupados = llenarBloque(size, block_count, ultimoBloque, instruccion);

	if(listaBloquesOcupados == NULL){
		//list_destroy_and_destroy_elements(listaBloquesOcupados, (void*) free);
		list_destroy(listaBloquesOcupados);
		freeMetadataBitacora(metadata);
		return -1;
	}

	list_add_all(metadata->blocks, listaBloquesOcupados);
	metadata->size += string_length(instruccion);
	metadata->block_count += list_size(listaBloquesOcupados);

	list_destroy(listaBloquesOcupados);

	if(modificarMetadataBitacora(metadata, id_tripulante)){
		return -1;
	} 
	pthread_mutex_unlock(&mutex_guardar_en_bitacora);
	//si fallo tengo q devolver -1}
	return 1;
}

char* obtenerBitacora(char* id_tripulante){

	MetadataBitacora* metadataB = leerMetadataBitacora(id_tripulante);
	char* lectura;

	if(metadataB->size > 0){
		lectura = leerBloques(metadataB->blocks, metadataB->size);		
	} else {
		lectura = string_new(); //Si no tiene nada en la bitacora devuelvo un string vacio.
	}

	freeMetadataBitacora(metadataB);
	return lectura;
}

int consumirRecursos(tipoRecurso recurso, int cantidadCaracteres){

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	if(metadataR->size == 0){
		miLogError("No hay recursos disponibles para consumir.");
		return -1;
	}

	int cantidadBytesLibres = bytesLibresUltimoBloque(metadataR->size, metadataR->block_count);
	int tamanioUltimoBloque = saberTamanioUltimobloque(cantidadBytesLibres);
	int posicionUltimoBloque = list_size(metadataR->blocks);
	int ultimoBloque;
	
	int bloquesEliminados = 0;

	while(cantidadCaracteres >= tamanioUltimoBloque && metadataR->block_count > 0){
	
		ultimoBloque = obtenerUltimoBloque(metadataR->blocks, posicionUltimoBloque);
		liberarBloque(ultimoBloque); //se modifica el bitmap
		bloquesEliminados++;
		posicionUltimoBloque--;
		cantidadCaracteres -= tamanioUltimoBloque; 
		metadataR->size -= tamanioUltimoBloque;
		metadataR->block_count--;
		tamanioUltimoBloque = tamanioBloque;
	}	
	
	if(cantidadCaracteres > metadataR->size){
		miLogError("No se pueden consumir mas recursos.");
	} else {
		//Me quedo un unico bloque que tiene espacio para seguir consumiendo recursos.
		metadataR->size -= cantidadCaracteres;	
		ultimoBloque = obtenerUltimoBloque(metadataR->blocks, posicionUltimoBloque);
		char* escrituraDelUltimoBloque = string_repeat(metadataR->caracter_llenado, tamanioUltimoBloque - cantidadCaracteres);
		reescribirBloque(ultimoBloque, escrituraDelUltimoBloque);	//Tiene que dejar en el ultimo bloque solo los caracteres del recurso.
		free(escrituraDelUltimoBloque);
	}
	
	t_list* bloquesNuevos = list_take(metadataR->blocks, list_size(metadataR->blocks)-bloquesEliminados);

	list_clean(metadataR->blocks);
	list_add_all(metadataR->blocks, bloquesNuevos);
	list_destroy(bloquesNuevos);
	
	if(modificarMetadataRecurso(metadataR, recurso)){
		return -1;
	} 

	return 1;
}

int obtenerUltimoBloque(t_list* blocks, int posicionUltimoBloque){

	int ultimoBloque;
					
	ultimoBloque = list_get(blocks, posicionUltimoBloque-1);

	return ultimoBloque;
}

int saberTamanioUltimobloque(int cantidadBytesLibres){
	
	return tamanioBloque - cantidadBytesLibres;
}


int desecharRecurso(tipoRecurso recurso){

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	int posicionUltimoBloque = list_size(metadataR->blocks);
	int ultimoBloque;
	
	while(posicionUltimoBloque > 0){
		ultimoBloque = obtenerUltimoBloque(metadataR->blocks, posicionUltimoBloque);
		liberarBloque(ultimoBloque);
		posicionUltimoBloque--;		
	}

	int resultado = eliminarArchivoBasura();
	freeMetadataRecurso(metadataR);

	return 1;
}

op_code enviarAvisoDeSabotaje(t_list* posicionesSabotaje, int socket_discordiador){
	
	t_pos* primerPosicion = primerPosicionSabotajeSinAtender(posicionesSabotaje);

	//No hay mas posiciones disponibles.
	if(primerPosicion == NULL){
		return SIN_POS_SABOTAJE;
	}
	
	t_paquete* paquete = crear_paquete(ALERTA_SABOTAJE);
    t_buffer* buffer;

	t_list* lista_mensajes = list_create();
	
	char* strX = string_itoa(primerPosicion->x);
	char* strY = string_itoa(primerPosicion->y);

	list_add(lista_mensajes, strX);
	list_add(lista_mensajes, strY);

	buffer = serializar_lista_strings(lista_mensajes);
    paquete ->buffer = buffer;
  
    enviar_paquete(paquete, socket_discordiador);

    //Aca deberia recibir la activación del protocolo FSCK?
	op_code codigo_operacion = recibir_operacion(socket_discordiador);

	list_destroy(lista_mensajes);
	free(primerPosicion);	
	free(strX);
	free(strY);
	return codigo_operacion;
}

t_pos* primerPosicionSabotajeSinAtender(t_list* posiciones){
	
	PosicionSabotaje* posicionSabotaje;
	t_pos* posicion = NULL;
	
	int _estaDesatendida(PosicionSabotaje* p) {
            return !(p->atendida);
    }
	posicionSabotaje = list_find(posiciones, (void*) _estaDesatendida);
	
	if(posicionSabotaje != NULL){
		posicion = posicionSabotaje->posicion;
		posicionSabotaje->atendida = true;	
	}
	
	return posicion;
}