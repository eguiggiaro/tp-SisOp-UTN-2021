#include "discordiador.h"
#include "serializacion_discordiador.h"
#include "servidor_discordiador.h"
#include "tripulante.h"

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

	puerto_discordiador = string_itoa(configuracion->puerto);
	
	/* 
		char* modulo;
		elegir_modulo();
		modulo = readline(">>");

		while(modulo_invalido(modulo)){
			printf("\nError! %s no es una opción correcta.\n", modulo);
			elegir_modulo();
			modulo = readline(">>");
		}

		if(strncmp(modulo,"1",1)==0){
		//Obtención datos para conexion con miram
		char* ip_miram = configuracion->ip_mi_ram_hq;
		char* puerto_miram = configuracion->puerto_mi_ram_hq;

		miLogInfo("Conectándose a MiRAM");
		printf("\nInicia conexion con MIRAM:\n");

		iniciar_conexion_miram(ip_miram, puerto_miram);

		char* ip_store = configuracion->ip_i_mongo_store;
		char* puerto_store = configuracion->puerto_i_mongo_store;

		miLogInfo("Conectándose a Store");
		printf("\nInicia conexion con STORE:\n");

		iniciar_conexion_store(ip_store, puerto_store);
		}
		else{
			miLogInfo("\nOpcion invalida\n");
		}

		if (pthread_create(&threadSERVER, NULL, (void*) iniciar_servidor_discordiador,
				NULL) != 0) {
			printf("Error iniciando servidor/n");
		}
	
	 */
	
	
	
	//Obtención datos para conexion con miram
	char* ip_miram = configuracion->ip_mi_ram_hq;
	char* puerto_miram = configuracion->puerto_mi_ram_hq;

	miLogInfo("Conectándose a MiRAM");
	printf("\nInicia conexion con MIRAM:\n");

	iniciar_conexion_miram(ip_miram, puerto_miram);

	char* ip_store = configuracion->ip_i_mongo_store;
	char* puerto_store = configuracion->puerto_i_mongo_store;

	miLogInfo("Conectándose a Store");
	printf("\nInicia conexion con STORE:\n");

	iniciar_conexion_store(ip_store, puerto_store);
	

	/* if (pthread_create(&threadSERVER, NULL, (void*) iniciar_servidor_discordiador,
			NULL) != 0) {
		printf("Error iniciando servidor/n");
	}

	pthread_join(threadSERVER, NULL);

	*/
	new_list=list_create();
	execute_list =list_create();
	blocked_io=list_create();
	
	consola();
	miLogInfo("Finalizó Discordiador\n");
	free(configuracion);
	miLogDestroy();

}

void consola(){
	char* input_consola = "INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4";
	char seps[]   = " \n";
	char *token;
 
	printf("Hola!\n");
	printf("Que desea hacer?\n");
	//*se podria poner ejemplitos de que puede hacer...

	while(1){

		//input_consola = readline(">>");
		token = strtok( input_consola, seps ); 
		if(token == NULL)
		{
			//todo: poner algo un poco mas profesional... o no :D
			printf("Emm... No escribiste nada...");
			return;
		}else
		{
			//tarea
		//	printf( " %s\n", token );
				//*la primer linea debe ser la tarea. dependiendo esta chequeamos si va a tener mas tokens o no.
				//!voy a hacer un enum con esto
				Comandos comando = (Comandos)token;
			switch (comando)
			{
				case INICIAR_PATOTA_COM:
					iniciar_patota(&token);//?like that right? with the &?
					break;

				case INICIAR_PLANIFICACION_COM:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case PAUSAR_PLANIFICACION_COM:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case LISTAR_TRIPULANTE_COM:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case EXPULSAR_TRIPULANTE_COM:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case OBTENER_BITACORA_COM:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case FIN:
					printf( "No implementado todavia. Gracias y vuelva pronto. :)\n" );
					break;
				case TEST_MENSAJES:
				printf( "1. Miriam\n" );
				printf( "2. Store\n" );

				char* test_modulo = readline(">>");

				if(test_modulo=="1"){
					consola_miriam();
				}else
				{
					consola_store();
				}
				
					break;
				default:
					printf( "No conozco ese comando, seguro que esta bien?\n" );
					break;
			}   

		}
	}
	
}
void iniciar_patota(char * token){
	char seps[]   = " \n";
	char* token2;
	char seps2[]="|";
	//* deberia ser la cantidad de tripulantes en la patota
	token = strtok( NULL, seps ); 
	if(token==NULL){
		//TODO: hacerlo  mas general este error y metodo aparte. Mandar ejemplos desde el meotodo q lo llame.
		printf( "Faltan datooos\n" );
		printf( "Ejemplo: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4 \n" );
		return;
	}
	int cant_trip=token;

	//*este deberia ser la lista de tareas de la patota. 
	//? debo mandar con discordiador no? no el primer tripulante o algo asi no?
	token = strtok( NULL, seps ); 
	//*posicion del primer tripulante
	token = strtok( NULL, seps ); 
	for( int i=0; cant_trip<=i;i++){
		
		pthread_t new_hilo_tripulante;
		Tripulante * new_tripulante;
		
		new_tripulante->estado= listo;
		new_tripulante->id_patota=1;//? y yo como se cual es mi patota? donde viene eso? Del nombre del archivo.
		new_tripulante->id_tripulante=i;

		if(token != NULL )
		{
			token2 = strtok( token, seps2 ); 
			new_tripulante->pos_x=(int)token2; //!can i do this? parse it directly 
			token2 = strtok( NULL, seps2 );
			new_tripulante->pos_y=(int)token2; //!can i do this? parse it directly 
			token = strtok( NULL, seps ); 
		}
		else {
			new_tripulante->pos_x=0;
			new_tripulante->pos_y=0;
		}
	int tripulante_hilo_id=pthread_create (&new_hilo_tripulante, NULL, iniciar_tripulante, (void *)&new_tripulante);

	list_add(new_list, &new_hilo_tripulante);
	}
		

	/* 
	TODO para hacer las listas tengo q hacer una especie de TBC? un struct con el id del hilo, su estado?, quantum, patota y nr de trip en esa patota, y...
	*/   
	//pthread_join(tripulante_hilo_id,NULL);
	//pthread_join(idHilo2,NULL);
	printf("Cerro hilo\n");
}
void iniciar_conexion_miram(char* ip_destino, char* puerto_destino) {

   int socket = crear_conexion(logger, ip_destino, puerto_destino);

	miLogInfo("Obtuve el socket y vale %d.\n", socket);

	if (socket == -1) {
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
	socket_miriam=socket;	
}
void consola_miriam(){
	//iniciamos consola
	char* opcion_seleccionada;
	iniciar_consola_miram();
	opcion_seleccionada = readline(">>");

	while(opcion_invalida_miram(opcion_seleccionada)){
		printf("\nError! %s no es una opción correcta.\n",opcion_seleccionada);
		iniciar_consola_miram();
		opcion_seleccionada = readline(">>");
	}

	printf("\nOpcion seleccionada: %s \n", opcion_seleccionada);

	//obtenemos el codigo de la operacion seleccionada por el usuario
	op_code operacion_seleccionada;
	operacion_seleccionada = convertir_codigo_operacion_miram(opcion_seleccionada);

	//enviamos accion a socket
	enviar_accion_seleccionada(operacion_seleccionada, socket_miriam);
}
void iniciar_conexion_store(char* ip_destino, char* puerto_destino) {

     //inicia conexion con destino
    int socket = crear_conexion(logger, ip_destino, puerto_destino);

	miLogInfo("Obtuve el socket y vale %d.\n", socket);

	if (socket == -1) {
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
	socket_store=socket;	
}
void consola_store(){
	//iniciamos consola
	char* opcion_seleccionada;
	iniciar_consola_store();
	opcion_seleccionada = readline(">>");

	while(opcion_invalida_store(opcion_seleccionada)){
		printf("\nError! %s no es una opción correcta.\n",opcion_seleccionada);
		iniciar_consola_store();
		opcion_seleccionada = readline(">>");
	}

	printf("\nOpcion seleccionada: %s \n", opcion_seleccionada);

	//obtenemos el codigo de la operacion seleccionada por el usuario
	op_code operacion_seleccionada;
	operacion_seleccionada = convertir_codigo_operacion_store(opcion_seleccionada);

	//enviamos accion a socket
	enviar_accion_seleccionada(operacion_seleccionada, socket_store);
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

void iniciar_consola_miram(){
	printf("Mensajes posibles:\n\n");
	printf("1) Expulsar tripulante\n");
	printf("2) Iniciar tripulante\n");
	printf("3) Informar tareas de patota\n");
	printf("4) Mover tripulante\n");
	printf("5) Tarea siguiente\n");
	printf("\nSeleccione el numero correspondiente\n\n");
}

void iniciar_consola_store(){
	printf("Mensajes posibles:\n\n");
	printf("1) Obtener bitacora\n");
	printf("2) Iniciar FSCK\n");
	printf("3) Generar oxigeno\n");
	printf("4) Enviar paquete de prueba\n");
	printf("\nSeleccione el numero correspondiente\n\n");
}

bool opcion_invalida_miram(char* opcion){
	return ((strncmp(opcion,"1",1)!=0) && (strncmp(opcion,"2",1)!=0) &&
			(strncmp(opcion,"3",1)!=0) && (strncmp(opcion,"4",1)!=0) &&
			(strncmp(opcion,"5",1)!=0));
}

bool opcion_invalida_store(char* opcion){
	return ((strncmp(opcion,"1",1)!=0) && (strncmp(opcion,"2",1)!=0) &&
			(strncmp(opcion,"3",1)!=0) && (strncmp(opcion,"4",1)!=0));
}

bool modulo_invalido(char* opcion){
	return ((strncmp(opcion,"1",1)!=0) && (strncmp(opcion,"2",1)!=0));
}

op_code convertir_codigo_operacion_store(char *codigo){
	op_code codigo_operacion;

	if(strncmp(codigo,"1",1)==0){
		codigo_operacion=OBTENER_BITACORA;
	}else if(strncmp(codigo,"2",1)==0){
		codigo_operacion=FSCK;
	}else if(strncmp(codigo,"3",1)==0){
		codigo_operacion = INFORMAR_TAREA;
	}else if(strncmp(codigo,"4",1)==0){
		codigo_operacion = PAQUETE;
	}

	return codigo_operacion;
}

op_code convertir_codigo_operacion_miram(char *codigo){
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

	return codigo_operacion;
}

void elegir_modulo(){
	printf("Elija el modulo con el que desea comunicarse:\n\n");
	printf("1) Mi-RAM\n");
	printf("2) I-Store\n");
	printf("\nSeleccione el numero correspondiente\n\n");
}

