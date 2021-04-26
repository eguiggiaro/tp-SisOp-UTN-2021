#include "store.h"

#define MODULE_NAME		 "I-Mongo-Store"
#define CONFIG_FILE_PATH "store/store.cfg"
#define LOG_FILE_PATH	 "store/store.log"

int main(int argc, char* argv[]) {

	t_log* logger;

	logger = iniciar_logger();

	log_info(logger, "Inició I-Mongo-Store.");
	helloWorld();

	if(leer_config(logger)){
		log_info(logger, "Error al iniciar I-Mongo-Store: No se encontró el archivo de configuración");
		log_destroy(logger);

		return EXIT_FAILURE;
	}
	/*Checkeo datos levantados del archivo de configuracion
	log_info(logger, "Punto de montaje: %s", configuracion->puntoMontaje);
	log_info(logger, "Puerto: %s", string_itoa(configuracion->puerto));
	log_info(logger, "Tiempo sincronizacion: %s", string_itoa(configuracion->tiempoSincro));
	 */



	log_info(logger, "Finalizó I-Mongo-Store.");
	log_destroy(logger);

	return EXIT_SUCCESS;
}

t_log* iniciar_logger(void){
	return log_create(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);
}

int leer_config(t_log* logger){

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");

	config_destroy(config);
	return EXIT_SUCCESS;
}
