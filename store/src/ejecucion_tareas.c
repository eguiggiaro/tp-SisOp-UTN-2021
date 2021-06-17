#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


void generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	//verificarMetadata(recurso); //Verifico si existe la metadata del recurso, sino lo creo. Por ej: Oxigeno.ims

	//leerMetadata(recurso); --> VA EN FILESYSTEM



	char * cadenaCaracteres = generarCadenaCaracteres(recurso, cantidadCaracteres);

	t_list* listaBloquesOcupados = list_create();
	
	if (recurso == OXIGENO){
		listaBloquesOcupados = llenarBloque(23, 3, 2, cadenaCaracteres);
	} else {
		listaBloquesOcupados = llenarBloque(45, 5, 7, cadenaCaracteres);
	}
	

	//modificarMetadata(metadata, listaBloquesOcupados);*/ --> VA EN FILESYSTEM
    

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

char * generarCadenaCaracteres(tipoRecurso recurso, int cantidadCaracteres){


    tipoRecurso opc = recurso;
    char * cadenaDeCaracteres;
	cadenaDeCaracteres = malloc(sizeof (char) * cantidadCaracteres);

		switch (opc)
		{
				case OXIGENO:
					printf( "OXIGENO\n" );
					cadenaDeCaracteres = string_repeat('O', cantidadCaracteres);
					break;

				case COMIDA:
					printf( "COMIDA\n" );
                    cadenaDeCaracteres = string_repeat('C', cantidadCaracteres);
					break;

				case BASURA:
					printf( "BASURA\n" );
					cadenaDeCaracteres = string_repeat('B', cantidadCaracteres);
					break;

				default:
					printf( "Error: recurso inexistente\n" );
					break;
		}
    return cadenaDeCaracteres;  
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

	t_list* listaBloquesOcupados = list_create();
	listaBloquesOcupados = llenarBloque(0, 0, 0, instruccion);
								//size, blockCount, ultimoBloque, instruccion
	//modificar metadata del tripulante. 
	
	//si fall{o tengo q devolver -1}
	return 1;
}
