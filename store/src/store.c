#include "store.h"
#include "file_system.h"
#include "request_analyzer.h"

#define MODULE_NAME "I-Mongo-Store"
#define CONFIG_FILE_PATH "cfg/store.cfg"
#define LOG_FILE_PATH "store.log"

int main(int argc, char* argv[]) {

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	
	if(leer_config()){
		miLogInfo("Error al iniciar I-Mongo-Store: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
	//Checkeo datos levantados del archivo de configuracion
	/*miLogInfo("Punto de montaje: %s", configuracion->puntoMontaje);
	miLogInfo("Puerto: %s", string_itoa(configuracion->puerto));
	miLogInfo("Tiempo sincronizacion: %s", string_itoa(configuracion->tiempoSincro));
	*/

	inicializar_store();

	miLogInfo("Finalizó I-Mongo-Store.");
	miLogDestroy();
	free(configuracion);

	return EXIT_SUCCESS;
}


int leer_config(void){

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
	//TODO: Levantar datos default de FS.

	/*configuracion->posicionesSabotaje = config_get_array_value(config, "POSICIONES_SABOTAJE");
	/*Para tratar las posiciones:
	char** posiciones = string_split(posicionesSabotaje[i], "|");
	t_pos posicion;
	posicion.x = atoi(posicion[0]);	
	posicion.y = atoi(posicion[1]);
	*/
	
	config_destroy(config);
	return EXIT_SUCCESS;
}


void inicializar_store (){
	
	miLogDebug("Inició I-Mongo-Store.");
	
	verificar_fs(configuracion);
	//subirblocksamemoria. (int* punteroABlocks = mmap de blocks.ims)   punteroABlocks[400] = 'O';
	levantar_servidor(atender_request_store, string_itoa(configuracion->puerto));
}
