#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


void generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	//verificarMetadata(recurso); //Verifico si existe la metadata del recurso, sino lo creo. Por ej: Oxigeno.ims

	leerMetadata(recurso);

	char * cadenaCaracteres = generarCadenaCaracteres(recurso, cantidadCaracteres);

	t_list* listaBloquesOcupados = list_create();
	listaBloquesOcupados = llenarBloque(2, 2, 2, cadenaCaracteres);

	//modificarMetadata(metadata, listaBloquesOcupados);*/
    

}


t_list* llenarBloque(int size, int blockCount, int ultimoBloque, char* cadenaCaracteres){
   
    int cantidadBytesLibres = bytesLibresUltimoBloque(size, blockCount);
    int posicionEnCadena = cantidadBytesLibres;

    if(cantidadBytesLibres > 0){

        char* cadenaHastaCantidad = truncarCadenaHastaCantidad(cadenaCaracteres, posicionEnCadena);
        escribirBloqueUsado(ultimoBloque, cantidadBytesLibres, cadenaHastaCantidad);
		posicionEnCadena++;
    }
    
	char* cadenaDesdeCantidad = truncarCadenaDesdeCantidad(cadenaCaracteres, posicionEnCadena); 

    t_list* bloquesEscritos = list_create(); 
	bloquesEscritos = escribirBloquesNuevo(cadenaDesdeCantidad);

    return bloquesEscritos;
    
}


void verificarMetadata(tipoRecurso recurso){
    //Verifico si existe el archivo metadata de ese, sino lo creo.
}

char* obtenerDireccionDeMetadata (tipoRecurso recurso){ //Devuelve la direccion de la metadata según el recurso que quiero

	char* direccionDeMetadata = string_new();
	char* nombreMetadata = stringFromRecurso(recurso);
	char* extencionDeMetadata = ".txt";
	string_append(&direccionDeMetadata, nombreMetadata);
	string_append(&direccionDeMetadata, extencionDeMetadata);

	return direccionDeMetadata;
}

char* stringFromRecurso(tipoRecurso f) //Obtiene un string de acuerdo al enum que le envío
{
    static const char *strings[] = { "oxigeno", "comida", "basura"};

    return strings[f];
}

int leerMetadata(tipoRecurso recurso){

	t_config* meta;
	metadata = malloc(sizeof(Metadata));

	char * direccionDeMetadata = obtenerDireccionDeMetadata(recurso);	

	meta = config_create(direccionDeMetadata);

	if(meta==NULL){
		return EXIT_FAILURE;
	}

	metadata->size = config_get_int_value(meta, "SIZE");
	metadata->block_count = config_get_int_value(meta, "{BLOCK_COUNT}");
	metadata->blocks = config_get_string_value(meta, "BLOCKS");
	metadata->caracter_llenado = config_get_string_value(meta, "CARACTER_LLENADO");
	//metadata->caracter_llenado = config_get_int_value(meta, "MD5");

	config_destroy(meta);
	return EXIT_SUCCESS;
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

