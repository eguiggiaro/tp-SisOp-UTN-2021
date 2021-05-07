#include "discordiador.h"
#include "serializacion_discordiador.h"

#define MODULE_NAME "Discordiador"
#define CONFIG_FILE_PATH "cfg/discordiador.cfg"
#define LOG_FILE_PATH "cfg/discordiador.log"

int main(){

	
	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, false, LOG_LEVEL_INFO);
	miLogInfo("Inició Discordiador.");

	if(leer_config()){
		miLogInfo("Error al iniciar Discordiador: No se encontró el archivo de configuración");
		miLogDestroy();
		return EXIT_FAILURE;
	}
		
	//Obtención datos para conexion con miram
	char* ip_miram = configuracion->ip_mi_ram_hq;
	char* puerto_miram = configuracion->puerto_mi_ram_hq ;


	miLogInfo("Conectándose a MiRAM");
	printf("\nInicia conexion con MIRAM:\n");
	iniciar_conexion(ip_miram, puerto_miram);

	//Obtención datos para conexion con store
	char* ip_store = configuracion->ip_i_mongo_store;
	char* puerto_store = configuracion->puerto_i_mongo_store;


	miLogInfo("Conectándose a Store");
	printf("\nInicia conexion con STORE:\n");
	iniciar_conexion(ip_store, puerto_store);

	
	miLogInfo("Finalizó Discordiador");
	free(configuracion);
	miLogDestroy();

}

void iniciar_conexion(char* ip_destino, char* puerto_destino) {

    //inicia conexion con destino
    int socket = crear_conexion(logger, ip_destino, puerto_destino);

	miLogInfo("Obtuve el socket y vale %d.\n", socket);

	if (socket == -1) {
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
	//iniciamos consola
	char* opcion_seleccionada;
	iniciar_consola();
	opcion_seleccionada = readline(">>");

	while(opcion_invalida(opcion_seleccionada)){
		printf("\nError! %s no es una opción correcta.\n",opcion_seleccionada);
		iniciar_consola();
		opcion_seleccionada = readline(">>");
	}

	printf("\nOpcion seleccionada: %s \n", opcion_seleccionada);

	//obtenemos el codigo de la operacion seleccionada por el usuario
	op_code operacion_seleccionada;
	operacion_seleccionada = convertir_codigo_operacion(opcion_seleccionada);

	//enviamos accion a socket
	enviar_accion_seleccionada(operacion_seleccionada, socket);
}



int leer_config(void) {

	t_config* config;
	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puntoMontaje = strdup(config_get_string_value(config, "PUNTO_MONTAJE"));
	configuracion->puerto = config_get_int_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	configuracion->ip_mi_ram_hq = strdup(config_get_string_value(config, "IP_MI_RAM_HQ"));
	configuracion->puerto_mi_ram_hq = strdup(config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
	configuracion->ip_i_mongo_store = strdup(config_get_string_value(config, "IP_I_MONGO_STORE"));
	configuracion->puerto_i_mongo_store = strdup(config_get_string_value(config, "PUERTO_I_MONGO_STORE"));
	configuracion->grado_multitarea = config_get_int_value(config, "GRADO_MULTITAREA");
	configuracion->algoritmo = strdup(config_get_string_value(config, "ALGORITMO"));
	configuracion->quantum = config_get_int_value(config, "QUANTUM");
	configuracion->duracion_sabotaje = config_get_int_value(config, "DURACION_SABOTAJE");
	configuracion->retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	
	config_destroy(config);
	return EXIT_SUCCESS;
}

void iniciar_consola(){
	printf("Te mostramos a continuación las opciones disponibles:\n\n");
	printf("1) Expulsar tripulante\n");
	printf("2) Iniciar tripulante\n");
	printf("3) Informar tareas de patota\n");
	printf("4) Mover tripulante\n");
	printf("5) Tarea siguiente\n");
	printf("6) Enviar paquete de prueba\n");
	printf("\n¿Qué querés hacer? Ingresá el número correspondiente\n\n");
}

bool opcion_invalida(char* opcion){
	return ((strncmp(opcion,"1",1)!=0) && (strncmp(opcion,"2",1)!=0) &&
			(strncmp(opcion,"3",1)!=0) && (strncmp(opcion,"4",1)!=0) &&
			(strncmp(opcion,"5",1)!=0) && (strncmp(opcion,"6",1)!=0));
}

op_code convertir_codigo_operacion(char *codigo){
	op_code codigo_operacion;

	if(strncmp(codigo,"1",1)==0){
		codigo_operacion=EXPULSAR_TRIPULANTE;
	}else if(strncmp(codigo,"2",1)==0){
		codigo_operacion=INICIAR_TRIPULANTE;
	}else if(strncmp(codigo,"3",1)==0){
		codigo_operacion = INFORMAR_TAREAS_PATOTA;
	}else if(strncmp(codigo,"4",1)==0){
		codigo_operacion = MOV_TRIPULANTE;
	}else if(strncmp(codigo,"5",1)==0){
		codigo_operacion = TAREA_SIGUIENTE;
	}
	else if(strncmp(codigo,"6",1)==0){
		codigo_operacion = PAQUETE;
	}

	return codigo_operacion;
}

