#include "discordiador.h"
#include "serializacion_discordiador.h"
#include "servidor_discordiador.h"
#include "tripulante.h"

#define MODULE_NAME "Discordiador"
#define CONFIG_FILE_PATH "cfg/discordiador.cfg"
#define LOG_FILE_PATH "cfg/discordiador.log"

int id_patota;

const char *comandos_table [] = { "INICIAR_PATOTA",
	"INICIAR_PLANIFICACION",
	"PAUSAR_PLANIFICACION",
	"LISTAR_TRIPULANTE",
	"EXPULSAR_TRIPULANTE",
	"OBTENER_BITACORA",
	"FIN",
	"TEST_MENSAJES", NULL };

int main(){
id_patota=0;
aux_id_tripulante = 0;
//enviar_tareas_miram("/home/utnso/tareas/tareasPatota5.txt",id_patota);
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

	//iniciar_conexion_store(ip_store, puerto_store);
	
/*
if (pthread_create(&threadSERVER, NULL, (void*) iniciar_servidor_discordiador,
			NULL) != 0) {
		printf("Error iniciando servidor/n");
	}
	pthread_join(threadSERVER, NULL);
	
*/
	
	new_list=list_create();
	execute_list = list_create();
	blocked_io=list_create();
	exit_list=list_create();
    ready_list=list_create();

	//inicializo semaforos
	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&mutexEXEC, 0, 1);
	sem_init(&mutexBLOCK, 0, 1);
	sem_init(&mutexEXIT, 0, 1);
		
	consola();
	miLogInfo("Finalizó Discordiador\n");
	//vaciar_listas();
	free(configuracion);
	miLogDestroy();

}

void consola(){
	char* input_consola = "INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4";
	char** list;
	printf("Hola!\n");
	printf("Que desea hacer?\n");
	//*se podria poner ejemplitos de que puede hacer...
	while(strcmp(input_consola,"FIN")!=0)
	{ 
		printf("%s\n",input_consola);	
		if(list == NULL)
		{
			//todo: poner algo un poco mas profesional... o no :D
			printf("Emm... No escribiste nada...");
			return;
		}else
		{
			//*la primer linea debe ser la tarea. dependiendo esta chequeamos si va a tener mas tokens o no.
			list=string_split(input_consola, " ");

			Comandos opc=find_enum_consola(list[0]);
			switch (opc)
			{
				case INICIAR_PATOTA_COM:
				printf( "Comando es Iniciar patota\n" );
					iniciar_patota(input_consola);
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
				case TEST_MENSAJES:
					printf( "1. Miram\n" );
					printf( "2. Store\n" );

					char test_modulo = readline(">>");

					if(test_modulo=="1"){
						consola_miram();
					}else
					{
						consola_store();
					}
				
					break;
				

				
				default:
					printf( "No conozco ese comando, seguro que esta bien?\n" );
					printf( "Reescribir, por favor\n" );
					input_consola = readline(">>");
					break;
			}
			printf("Siguiente comando?\n");
			input_consola = readline(">>");	
		}
	}
}
void iniciar_patota(char* comando){
	//INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4
	pthread_t new_hilo_tripulante;
	t_list* lista_mensajes=list_create(), *mensajes_respuesta= list_create();
	Tripulante_disc trip_hilo;

	char* string_tareas;
	char* string_posiciones = string_new();
	char** list;

	list=string_split(comando, " ");
	string_tareas=leer_tareas_txt(list[2]);//leemos las tareas y las traemos como un solo string

	int cantidad_trip=atoi(list[1]);
	for(int i=0; i<=cantidad_trip;i++){//string con todas las posiciones
		
		if(sizeof(list)>i+1 && list[i+3] != NULL)
		{
			string_append_with_format(&string_posiciones, "%s;", list[i+3]);
		}
		else {
			string_append(&string_posiciones, "0|0;");
		}
	}

	list_add(lista_mensajes,string_tareas);
	list_add(lista_mensajes,list[1]);
	list_add(lista_mensajes,string_posiciones);
	mensajes_respuesta=iniciar_patota_miram(socket_miram,lista_mensajes);

	char* patota_id=list_get(mensajes_respuesta,0);
	
	for(int i=0; i<=cantidad_trip;i++){
		Tripulante* new_tripulante= malloc(sizeof(Tripulante));

		new_tripulante->id_patota=atoi(patota_id);
		//new_tripulante->pos_x=atoi(list_get(mensajes_respuesta,1));
		//new_tripulante->pos_y=atoi(list_get(mensajes_respuesta,2));

		//*creo hilo y agrego a "new"
		pthread_create (&new_hilo_tripulante, NULL, inicializar_tripulante, (void *)&new_tripulante);
		trip_hilo.id_hilo =new_hilo_tripulante;
		trip_hilo.quantum=0;
		list_add(new_list, &trip_hilo);

	}
	
	list_destroy(lista_mensajes);	
	printf ("DONE!");
}

void vaciar_listas(){
	//! not so sure this will work running...
	void* list_value;
	Tripulante_disc* hilo_tripulante;
	for(int i=0;list_size(new_list)<i;i++){
		list_value =list_get(new_list, i);
		hilo_tripulante=(Tripulante_disc*)list_value;
		pthread_join(hilo_tripulante->id_hilo,NULL); 
	}
	list_destroy(new_list);
	printf("Cerro hilo\n");
}

char* leer_tareas_txt(char* direccion_txt){
	// ejemplo de tarea TAREA PARAMETROS;POS X;POS Y;TIEMPO
	char* lista_tareas =  string_new();
	char line[100];

	FILE* file_tarea=fopen(direccion_txt,"r");
	if (file_tarea == NULL ) {
		printf ("Error al abrir el fichero");}
	while(!feof(file_tarea))
	{
		fgets(line,sizeof(line),file_tarea);
		
		string_append_with_format(&lista_tareas, "%s|", line);
	}

	fclose(file_tarea);
	return lista_tareas;

}

void tripulante_listo(Tripulante* trip){
	//Se pasa tripulante a cola de READY.
	sem_wait(&mutexREADY);
    list_add(ready_list, trip);
    sem_post(&mutexREADY);
	miLogInfo("Se pasa el tripulante a la cola de READY");
}


void iniciar_conexion_miram(char* ip_destino, char* puerto_destino) {

   int socket = crear_conexion(logger, ip_destino, puerto_destino);

	miLogInfo("Obtuve el socket y vale %d.\n", socket);

	if (socket == -1) {
		miLogInfo("No fue posible establecer la conexión del socket solicitado.\n");
		exit(3);
	}
	socket_miram=socket;	
}
void consola_miram(){
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
	enviar_accion_seleccionada(operacion_seleccionada, socket_miram);
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



Comandos find_enum_consola(char *sval)
{
    Comandos result=INICIAR_PATOTA_COM; /* value corresponding to etable[0] */
    int i=0;
    for (i=0; comandos_table[i]!=NULL; ++i, ++result)
        if (0==strcmp(sval, comandos_table[i])) return result;
    return -1;
}