#include "ejecucion_tareas.h"
#include "file_system.h"
#include "store.h"


void generarRecursos(tipoRecurso recurso, int cantidadCaracteres){

	/*verificarMetadata(recurso); //Verifico si existe la metadata del recurso, sino lo creo. Por ej: Oxigeno.ims

	//leerMetadata();

	char * cadenaCaracteres = generarCadenaCaracteres(recurso, cantidad);

	t_list* listaBloquesOcupados = list_create();
	listaBloquesOcupados = llenarBloque(2, 2, 2, cadenaCaracteres);

	//modificarMetadata(metadata, listaBloquesOcupados);*/
    

}


t_list* llenarBloque(int size, int blockCount, int ultimoBloque, char* cadenaCaracteres){
   
    int cantidadBytesLibres = bytesLibresUltimoBloque(size, blockCount);

    char* cadenaDesdeCantidad = truncarCadenaDesdeCantidad(cadenaCaracteres, cantidadBytesLibres); 

    if(cantidadBytesLibres > 0){

        char* cadenaHastaCantidad = truncarCadenaHastaCantidad(cadenaCaracteres, cantidadBytesLibres);
        escribirBloqueUsado(ultimoBloque, cantidadBytesLibres, cadenaHastaCantidad);
    }
    
    t_list* bloquesEscritos = list_create(); 
	bloquesEscritos = escribirBloquesNuevo(cadenaDesdeCantidad);

    return bloquesEscritos;
    
}

/*
void verificarMetadata(tipoRecurso recurso){
    //Verifico si existe el archivo metadata de ese, sino lo creo.
}

int leerMetadata(void){

	t_config* config;
	metadata = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	metadata->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	metadata->puerto = config_get_int_value(config, "PUERTO");
	metadata->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	metadata->blockSizeDefault = config_get_int_value(config, "BLOCK_SIZE");
	metadata->blocksQtyDefault = config_get_int_value(config, "BLOCKS");
	metadata->posicionesSabotaje = config_get_string_value(config, "POSICIONES_SABOTAJE"); //POSICIONES_SABOTAJE=[1|1, 2|2, 3|3, 4|4, 5|5, 6|6, 7|7]
	
	t_list* posiciones = obtenerListaSabotaje(configuracion->posicionesSabotaje);

	config_destroy(config);
	return EXIT_SUCCESS;
}
*/
char * generarCadenaCaracteres(tipoRecurso recurso, int cantidad){


    tipoRecurso opc = recurso;
    char * cadenaDeCaracteres;
	cadenaDeCaracteres = malloc(sizeof (char) * cantidad);

		switch (opc)
		{
				case OXIGENO:
					printf( "OXIGENO\n" );
					cadenaDeCaracteres = string_repeat('O', cantidad);
					break;

				case COMIDA:
					printf( "COMIDA\n" );
                    cadenaDeCaracteres = string_repeat('C', cantidad);
					break;

				case BASURA:
					printf( "BASURA\n" );
					cadenaDeCaracteres = string_repeat('B', cantidad);
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

char * truncarCadenaDesdeCantidad(char* cadenaCaracteres, int cantidad){return cadenaCaracteres;}
char * truncarCadenaHastaCantidad(char* cadenaCaracteres, int cantidad){return cadenaCaracteres;}

