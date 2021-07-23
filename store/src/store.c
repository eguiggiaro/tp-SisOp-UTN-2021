#include "store.h"
#include "file_system.h"
#include "request_analyzer.h"

#define MODULE_NAME "I-Mongo-Store"
#define CONFIG_FILE_PATH "cfg/store.cfg"
#define LOG_FILE_PATH "cfg/store.log"

void signalHandler(int signal){
	
	switch(signal) {
		case SIGUSR1:
			miLogInfo("Me llego la señal %d, atiendo el sabotaje", signal);	
			atenderSabotaje();	
			break;
		case SIGINT:
			miLogInfo("Se forzo el cierre del I-Mongo-Store.");
			finalizarStore();
			exit(130);	//Control+C
			break;
		default:
			break;
	}	
}

void syncPeriodico(void){

    int tamanioBlocks = cantidadBloques * tamanioBloque;

	while(1){
        sleep(configuracion->tiempoSincro);
        msync(punteroBlocks, tamanioBlocks, 0); //Fuerzo la actualización del bitmap en el archivo.
		miLogInfo("Se forzo la sincronizacion del archivo Blocks.ims a disco. La proxima sincronizacion sera dentro de %d segundos.", configuracion->tiempoSincro);
    }   
}

int main(int argc, char* argv[]) {

	//Inicio el log en un thread... :O
	miLogInitMutex(LOG_FILE_PATH, MODULE_NAME, true, LOG_LEVEL_INFO);

	//Configuro el signal SIGUSR1 para iniciar el sabotaje.
	signal(SIGUSR1, signalHandler);
	signal(SIGINT, signalHandler);

	if(leerConfig()){
		miLogInfo("Error al iniciar I-Mongo-Store: No se encontro el archivo de configuracion");
		miLogDestroy();
		return EXIT_FAILURE;
	}

	inicializarStore();
	
	return EXIT_SUCCESS;
}


int leerConfig(void){

	configuracion = malloc(sizeof(Configuracion));

	config = config_create(CONFIG_FILE_PATH);

	if(config==NULL){
		return EXIT_FAILURE;
	}

	configuracion->puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE");
	configuracion->puerto = config_get_string_value(config, "PUERTO");
	configuracion->tiempoSincro = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	configuracion->blockSizeDefault = config_get_int_value(config, "BLOCK_SIZE");
	configuracion->blocksQtyDefault = config_get_int_value(config, "BLOCKS");
	configuracion->posicionesSabotaje = config_get_string_value(config, "POSICIONES_SABOTAJE"); //POSICIONES_SABOTAJE=[1|1, 2|2, 3|3, 4|4, 5|5, 6|6, 7|7]	
	configuracion->ipDiscordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	configuracion->puertoDiscordiador = config_get_string_value(config, "PUERTO_DISCORDIADOR");

	return EXIT_SUCCESS;
}

void inicializarStore(void){
	
	miLogInfo("==== Inicio I-Mongo-Store =====");

	inicializarParametrosFS();

	if (!verificarFS()){
		miLogInfo("No se encontro un FILE SYSTEM valido, se procede a crearlo con los parametros default");
		if(verificarExistenciaPuntoMontaje(puntoMontaje)){
			borrarTodosLosArchivos(puntoMontaje);
		}
		crearArbolDirectorios();
		crearSuperbloque();
		crearBlocks();
		miLogInfo("FILE SYSTEM creado exitosamente");
	}

	leerSuperbloque();
	subirBlocksAMemoria();
	inicializarPosicionesSabotaje();
	inicializarSemaforos();

	//Inicio el thread de msync para bloques periodico.
	pthread_t th_msync;
	pthread_create(&th_msync, NULL, &syncPeriodico, NULL);	
	
	//protocoloFsck();

	levantar_servidor(atender_request_store, configuracion->puerto);	

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

void inicializarSemaforos(){
	pthread_attr_init(&attrEjecucionSabotaje);
    pthread_mutex_init(&mutexEjecucionSabotaje, NULL);
    pthread_cond_init(&condEjecucionSabotaje, NULL);
	esperaSabotaje = 0;
}

void inicializarPosicionesSabotaje(){
	
	t_list* posiciones = obtenerListaSabotaje(configuracion->posicionesSabotaje);	
	PosicionSabotaje* posicionSabotaje;

	posicionesSabotaje = list_create();
	for(int i = 0; i < list_size(posiciones); i++){
		posicionSabotaje = malloc(sizeof(PosicionSabotaje));		
		posicionSabotaje->posicion = (t_pos*) list_get(posiciones, i);
		posicionSabotaje->atendida = false;

		list_add(posicionesSabotaje, posicionSabotaje);
//		free(posicionSabotaje->posicion);
//		free(posicionSabotaje);	
	}
	//TEST
	/*for(int i = 0; i < list_size(posicionesSabotaje); i++){
		posicionSabotaje = (PosicionSabotaje*) list_get(posicionesSabotaje, i);	
		printf("x:%s, y:%s\n", string_itoa(posicionSabotaje->posicion->x), string_itoa(posicionSabotaje->posicion->y));
	}*/

	list_destroy(posiciones);
}

int iniciarConexionDiscordiador()
{	//inicia conexion con destino
	int socket = crear_conexion(logger, configuracion->ipDiscordiador, configuracion->puertoDiscordiador);
	
	if (socket == -1) {
		miLogError("No fue posible establecer la conexion del socket solicitado.");
		exit(3);
	}
	miLogInfo("Conexion con discordiador iniciada correctamente. (Socket: %d).", socket);

	return socket;
}

void atenderSabotaje(){
	//0.Inicio la conexion con el Discordiador.
	int socket_discordiador = iniciarConexionDiscordiador();
	
	//1.Avisar a discordiador -> enviar posiciones de sabotaje (configuracion)
	op_code respuestaDiscordiador = enviarAvisoDeSabotaje(posicionesSabotaje, socket_discordiador); 
	if (respuestaDiscordiador == OK) {
		miLogInfo("El Discordiador recibio el alerta de sabotaje correctamente. Esperando activacion de protocolo FSCK...");
		//2.Esperar a que el Discordiador me active el protocolo fsck.
		pthread_mutex_lock(&mutexEjecucionSabotaje); 
		while(!esperaSabotaje) { 
			pthread_cond_wait(&condEjecucionSabotaje, &mutexEjecucionSabotaje); 
		}
		pthread_mutex_unlock(&mutexEjecucionSabotaje);
		
		//3.Llamar al protocolo de sabotaje	
		protocoloFsck();
		esperaSabotaje = 0;
	} else if (respuestaDiscordiador == FAIL){
        miLogError("ERROR: Fallo la respuesta del discordiador al pedido de resolucion de sabotaje.");
	}
	//t_buffer* buffer = (t_buffer*)recibir_buffer(socket_discordiador);
}

void protocoloFsck(){
	
	miLogInfo("Se da inicio al protocolo FSCK.");
	
	//Analizar sabotaje en Superbloque
	if(!verificarCantidadBloques()){
		miLogInfo("Verificacion de cantidad de bloques en el Superbloques.ims finalizada con exito.");
	}

	if(!verificarBitmap()){
		miLogInfo("Verificacion del Bitmap en el Superbloques.ims finalizada con exito.");
	}

	//Analizar sabotaje en files
	if(!verificarBlockCount()){
		miLogInfo("Verificacion de BlockCount en los Files de Recursos finalizada con exito.");
	}

	if(!verificarSizeEnFile()){
		miLogInfo("Verificacion de Size en los Files de Recursos finalizada con exito.");
	}

	if(!verificarBlocks()){
		miLogInfo("Verificacion de Bloques en los Files de Recursos finalizada con exito.");
	}
	


	miLogInfo("Finalizo el protocolo FSCK.");
	
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
		list_add(listaPosicionesSabotaje, posicion);

		largo-=3;
		largo-=strlen(strPosicion[0]);
		largo-=strlen(strPosicion[1]);
	
		liberar_array(strPosicion);	
	}
	//TEST
	/*for(int i = 0; i < list_size(listaPosicionesSabotaje); i++){
		posicion = (t_pos*) list_get(listaPosicionesSabotaje, i);	
		printf("x:%s, y:%s\n", string_itoa(posicion->x), string_itoa(posicion->y));
	}*/

	liberar_array(posicionesSabotaje);
	
	return listaPosicionesSabotaje;
}

void finalizarStore(){
	miLogInfo("==== Finalizo I-Mongo-Store ====");
	
	pthread_mutex_destroy(&mutexEjecucionSabotaje);
	pthread_cond_destroy(&condEjecucionSabotaje);
	pthread_attr_destroy(&attrEjecucionSabotaje);

	list_destroy_and_destroy_elements(posicionesSabotaje, (void*) posicion_sabotaje_destroy);
	config_destroy(config);
	finalizarFS();
	miLogDestroy();	
	free(configuracion);	
}