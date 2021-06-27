#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


int generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	char caracter = cualEsMiCaracter(recurso);
	char* strCaracter = string_new();
	strCaracter = string_repeat(caracter, 1);
	char * cadenaCaracteres = generarCadenaCaracteres(caracter, cantidadCaracteres);

	int size = metadataR->size;
	int block_count = metadataR->block_count;
	int posicionUltimoBloque = list_size(metadataR->blocks);
	int ultimoBloque = 0;
	
	if (posicionUltimoBloque != 0){
		posicionUltimoBloque--;
		ultimoBloque = list_get(metadataR->blocks, posicionUltimoBloque);
	} 
	 
	t_list* listaBloquesOcupados = list_create();
	
	
	listaBloquesOcupados = llenarBloque(size, block_count, ultimoBloque, cadenaCaracteres);

	list_add_all(metadataR->blocks, listaBloquesOcupados);
	metadataR->size += cantidadCaracteres;
	metadataR->block_count += list_size(listaBloquesOcupados);
	metadataR->caracter_llenado = strCaracter;

	free(cadenaCaracteres);
	free(strCaracter);

	if(modificarMetadataRecurso(metadataR, recurso)){
		return -1;
	} 
    
	return 1;

}


t_list* llenarBloque(int size, int blockCount, int ultimoBloque, char* cadenaCaracteres){
   
    int cantidadBytesLibres = bytesLibresUltimoBloque(size, blockCount);
    int posicionEnCadena = cantidadBytesLibres;
	t_list* bloquesEscritos = list_create(); 

    if(cantidadBytesLibres > 0){

        char* cadenaHastaCantidad = truncarCadenaHastaCantidad(cadenaCaracteres, posicionEnCadena);
        escribirBloqueUsado(ultimoBloque, cantidadBytesLibres, cadenaHastaCantidad);

		if(string_length(cadenaHastaCantidad) == string_length(cadenaCaracteres)){
			free(cadenaHastaCantidad);
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

    char * cadenaDeCaracteres;
	cadenaDeCaracteres = malloc(sizeof (char) * cantidadCaracteres);

	cadenaDeCaracteres = string_repeat(caracter, cantidadCaracteres);
		
    return cadenaDeCaracteres;  
}

char cualEsMiCaracter(tipoRecurso recurso){

	char caracter;
	switch (recurso)
	{
		case OXIGENO:
			printf( "OXIGENO\n" );
			caracter = 'O';
			break;

		case COMIDA:
			printf( "COMIDA\n" );
			caracter = 'C';
			break;

		case BASURA:
			printf( "BASURA\n" );
			caracter = 'B';
			break;

		default:
			printf( "Error: recurso inexistente\n" );
			break;
	}
	return caracter;
}


int bytesLibresUltimoBloque(int size, int blockCount){

	int bytesLibres = blockCount * configuracion -> blockSizeDefault - size;

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

	t_list* listaBloquesOcupados = list_create();
	listaBloquesOcupados = llenarBloque(size, block_count, ultimoBloque, instruccion);

	list_add_all(metadata->blocks, listaBloquesOcupados);
	metadata->size += string_length(instruccion);
	metadata->block_count += list_size(listaBloquesOcupados);

	list_destroy(listaBloquesOcupados);

	if(modificarMetadataBitacora(metadata, id_tripulante)){
		return -1;
	} 
	pthread_mutex_unlock(&mutex_guardar_en_bitacora);
	//si fall{o tengo q devolver -1}
	return 1;
}

char* obtenerBitacora(char* id_tripulante){

	MetadataBitacora* metadataB = malloc(sizeof(MetadataBitacora));
	
	metadataB = leerMetadataBitacora(id_tripulante);

	char* lectura = leerBloques(metadataB->blocks, metadataB->size);
	printf("%s",lectura);

	return lectura;
	
}

/*int consumirRecurso(tipoRecurso recurso, int cantidadCaracteres){

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	int size = metadataR->size;
	int block_count = metadataR->block_count;
	t_list* blocks = metadataR->blocks;

	int posicionUltimoBloque = list_size(blocks);
	int ultimoBloque = obtenerUltimoBlouque(metadataR, posicionUltimoBloque);

	size -= cantidadCaracteres; //cambio el tamaño del archivo

	int tamanioUltimoBloque = saberTamanioUltimobloque(ultimoBloque);
	int bloquesEliminados = 0;

	for (bloquesEliminados ; tamanioUltimoBloque <= cantidadCaracteres; bloquesEliminados ++){
	
		liberarBloque(ultimoBloque); //se modifica el bitmap
		cantidadCaracteres -= tamanioBloque;  
		posicionUltimoBloque --; //modifico el puntero de blocks 
		block_count --; //resto un bloque al blockCount
		ultimoBloque = obtenerUltimoBlouque(blocks, posicionUltimoBloque); 
		tamanioUltimoBloque = saberTamanioUltimobloque();

	}

	blocks = list_take(list_size(blocks)-bloquesEliminados);

}

int obtenerUltimoBlouque(t_list* blocks, int posicionUltimoBloque){
	if (posicionUltimoBloque != 0){
		posicionUltimoBloque--;
		int ultimoBloque = list_get(blocks, posicionUltimoBloque);
		return ultimoBloque;
	} 
	return 0;
}*/

// enviarAvisoDeSabotaje(char* posicionesSabotaje); AVISO_SABOTAJE [posicionesSabotaje];

/*
int desecharBasura(){
	
	//liberarBloque(ultimoBloque);
	//borrarArchivoBasura();
}
*/