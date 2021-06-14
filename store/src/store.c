#include "store.h"
#include "file_system.h"
#include "request_analyzer.h"

#define MODULE_NAME "I-Mongo-Store"
#define CONFIG_FILE_PATH "cfg/store.cfg"
#define LOG_FILE_PATH "store.log"

int main(int argc, char* argv[]) {

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	
	if(leerConfig()){
		miLogInfo("Error al iniciar I-Mongo-Store: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	inicializarStore();

	miLogInfo("Finalizó I-Mongo-Store.");
	miLogDestroy();
	free(configuracion);

	return EXIT_SUCCESS;
}


int leerConfig(void){

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	configuracion->blockSizeDefault = config_get_int_value(config, "BLOCK_SIZE");
	configuracion->blocksQtyDefault = config_get_int_value(config, "BLOCKS");
	configuracion->posicionesSabotaje = config_get_string_value(config, "POSICIONES_SABOTAJE"); //POSICIONES_SABOTAJE=[1|1, 2|2, 3|3, 4|4, 5|5, 6|6, 7|7]
	
	//t_list* posiciones = obtenerListaSabotaje(configuracion->posicionesSabotaje);

	config_destroy(config);
	return EXIT_SUCCESS;
}

void inicializarParametrosFS(void){
	puntoMontaje = strdup(configuracion->puntoMontaje);
	pathSuperbloque = string_from_format("%s/%s", puntoMontaje, SUPERBLOQUE_FILE); //ej: /home/utnso/mnt/SuperBloque.ims
	pathBlocks = string_from_format("%s/%s", puntoMontaje, BLOCKS_FILE);
	pathFiles = string_from_format("%s/%s", puntoMontaje, "Files");
	pathBitacoras = string_from_format("%s/%s", puntoMontaje, "Files/Bitacoras");

	tamanioBloque = configuracion->blockSizeDefault;
	cantidadBloques = configuracion->blocksQtyDefault;
}

void inicializarStore(void){
	
	miLogDebug("Inició I-Mongo-Store.");
	
	inicializarParametrosFS();

	if (!verificarFS()){
		crearArbolDirectorios();
		crearSuperbloque();
		crearBlocks();
	}

	leerSuperbloque();
	subirBlocksAMemoria();
	//leerArchivosMetadata();
	levantar_servidor(atender_request_store, string_itoa(configuracion->puerto));
}


t_list* obtenerListaSabotaje(char* strPosicionesSabotaje){
	
	char** posicionesSabotaje = string_get_string_as_array(strPosicionesSabotaje);
	int largo = strlen(strPosicionesSabotaje) - 1;

	t_list* listaPosicionesSabotaje = list_create();

	t_pos* posicion;

	for(int i = 0;largo > 0; i++){
		char** strPosicion =  string_split(posicionesSabotaje[i], "|");
		posicion = malloc(sizeof(t_pos));
		posicion->x = atoi(strPosicion[0]);	
		posicion->y = atoi(strPosicion[1]);
		list_add(listaPosicionesSabotaje,(void*) posicion);

		largo-=3;
		largo-=strlen(strPosicion[0]);
		largo-=strlen(strPosicion[1]);
	}
	//Test
	/*for(int i = 0; list_size(listaPosicionesSabotaje) > i; i++){
		posicion = (t_pos*) list_get(listaPosicionesSabotaje, i);
		printf("x:%s, y:%s\n", string_itoa(posicion->x), string_itoa(posicion->y));
	}*/
	free(posicion);
	return listaPosicionesSabotaje;
}
