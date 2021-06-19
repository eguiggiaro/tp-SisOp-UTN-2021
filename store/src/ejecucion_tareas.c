#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


int generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	//verificarMetadata(recurso); //Verifico si existe la metadata del recurso, sino lo creo. Por ej: Oxigeno.ims

	MetadataRecurso* metadataR = leerMetadataRecurso(recurso);

	char caracter = cualEsMiCaracter(recurso);
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
	metadataR->caracter_llenado = &caracter;

	free(cadenaCaracteres);

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
			return bloquesEscritos;
		}
	}
    
	char* cadenaDesdeCantidad = truncarCadenaDesdeCantidad(cadenaCaracteres, posicionEnCadena);  
	bloquesEscritos = escribirBloquesNuevo(cadenaDesdeCantidad);

	
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
								
	if(!modificarMetadataBitacora(metadata, id_tripulante)){
		return -1;
	} 
	
	//si fall{o tengo q devolver -1}
	return 1;
}
