#include "store.h"

#define MODULE_NAME "I-Mongo-Store"
#define CONFIG_FILE_PATH "../store.cfg"
#define CONFIG_FILE_PATH_ECLIPSE "store/store.cfg"
#define LOG_FILE_PATH "../store.log"

int main(int argc, char* argv[]) {

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
	miLogInfo("Inició I-Mongo-Store.");


	if(leer_config()){
		miLogInfo("Error al iniciar I-Mongo-Store: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
	/*Checkeo datos levantados del archivo de configuracion
	miLogInfo("Punto de montaje: %s", configuracion->puntoMontaje);
	miLogInfo("Puerto: %s", string_itoa(configuracion->puerto));
	miLogInfo("Tiempo sincronizacion: %s", string_itoa(configuracion->tiempoSincro));
	 */


	miLogInfo("Finalizó I-Mongo-Store.");
	miLogDestroy();


	return EXIT_SUCCESS;
}


int leer_config(void){

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		//Para debuggear desde eclipse.
		config = config_create(CONFIG_FILE_PATH_ECLIPSE);
		if(config==NULL){
			return EXIT_FAILURE;
		}
	}

	configuracion->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");

	config_destroy(config);
	return EXIT_SUCCESS;
}
