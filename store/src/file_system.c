#include "file_system.h"
#include "ejecucion_tareas.h"


bool verificarFS(void){
		return !(fopen(pathSuperbloque, "r") == NULL || fopen(pathBlocks, "r") == NULL);
}

void crearDirectorio(char* path){

	if(mkdir(path, 0777) == -1){
		printf("No se pudo crear el directorio %s.\n", path);
		exit(EXIT_FAILURE);
	}
	miLogInfo("Agrego directorio %s.", path);
}

void crearArbolDirectorios(void){
	
	DIR* dirMontaje = opendir(puntoMontaje);
	DIR* dirFiles = opendir(pathFiles);
	DIR* dirBitacoras = opendir(pathBitacoras);
	
	if(!dirMontaje){
		crearDirectorio(puntoMontaje);
	}

	if(!dirFiles){
		crearDirectorio(pathFiles);			
	} 

	if(!dirBitacoras){
		crearDirectorio(pathBitacoras);
	}

	closedir(dirMontaje);
	closedir(dirFiles);
	closedir(dirBitacoras);
}

void borrarTodosLosArchivos(char* path){

	char* comando = string_new();
	comando = string_from_format("%s %s/%s", "rm -r", path, "*");

	int res = system(comando);

	if(!res){
		miLogInfo("Borró la estructura de file system existente en el punto de montaje %s", path);
	} else {
		miLogInfo("No pudo borrar la estructura de file system existente en el punto de montaje %s", path);
		exit(EXIT_FAILURE);
	}	
}

void crearSuperbloque(void){
	
	FILE* archivoSuperbloque = fopen(pathSuperbloque, "w+");
	if(archivoSuperbloque==NULL){
		miLogInfo("No se pudo crear el archivo de Superbloque.ims.");
		exit(EXIT_FAILURE);
	}
	
	fwrite(&tamanioBloque, 1 , sizeof(uint32_t), archivoSuperbloque);
	fwrite(&cantidadBloques, 1 , sizeof(uint32_t), archivoSuperbloque);	
	inicializarBitmap(archivoSuperbloque);
	
	fclose(archivoSuperbloque);
}

void crearBlocks(void){
	
	FILE* archivoBlocks = fopen(pathBlocks, "w+");
	int tamanioBlocks = cantidadBloques * tamanioBloque;

	if(archivoBlocks==NULL){
		miLogInfo("No se pudo crear el archivo de Blocks.ims.");
		exit(EXIT_FAILURE);
	}
	//Dejo el archivo de bloques con el tamaño final que va a tener.
	if(!ftruncate(fileno(archivoBlocks), tamanioBlocks)){
		miLogInfo("Se creo el archivo Blocks.ims correctamente, con un tamaño inicial: %s", string_itoa(tamanioBlocks));
	}

	fclose(archivoBlocks);
}

void inicializarBitmap(FILE* archivoSuperbloque){
	//Reservo memoria para el bitmap y lo inicializo en cero. El bitmap es a nivel bits, por eso lo divido por 8.
	void* bitmap = calloc(cantidadBloques/8, 1);

	fwrite(bitmap, 1, cantidadBloques/8, archivoSuperbloque);
	miLogInfo("Inicializó el bitmap del archivo SuperBloque.");
	
	free(bitmap);
}

void leerSuperbloque(void){

	int archivoSuperbloque = open(pathSuperbloque, O_RDWR);
	int tamanioSuperbloque = cantidadBloques/8 + sizeof(uint32_t) * 2;

	char* punteroSuperbloque = mmap(NULL, tamanioSuperbloque, PROT_READ | PROT_WRITE, MAP_SHARED, archivoSuperbloque, 0);
	char* punteroBitmap = punteroSuperbloque + sizeof(uint32_t) * 2;

	memcpy(&tamanioBloque, punteroSuperbloque, sizeof(uint32_t));
	memcpy(&cantidadBloques, punteroSuperbloque + sizeof(uint32_t), sizeof(uint32_t));
	bitmap = bitarray_create_with_mode(punteroBitmap, cantidadBloques/8, MSB_FIRST);

	//Test
	/*bool bit;
	for(int i=0; i<cantidadBloques; i++){
		bit = bitarray_test_bit(bitmap, i);	
	}*/
	close(archivoSuperbloque);
}

void leerSuperBloqueSinMapear(void){
	FILE* archivoSuperbloque = fopen(pathSuperbloque, "r");
	uint32_t* buffer = malloc(sizeof(uint32_t));
	
	fseek(archivoSuperbloque, 0, SEEK_SET);
	fread(buffer, sizeof(uint32_t), 1, archivoSuperbloque);
	tamanioBloque = *buffer;

	fseek(archivoSuperbloque, 0, SEEK_CUR);
	fread(buffer, sizeof(uint32_t), 1, archivoSuperbloque);
	cantidadBloques = *buffer;

	//Cargar el bitmap en un bitarray.
	/*char* bufferBitmap = malloc(cantidadBloques/8);
	fseek(archivoSuperbloque, 0, SEEK_CUR);
	fread(bufferBitmap, cantidadBloques/8, 1, archivoSuperbloque);
	bitmap = bitarray_create_with_mode(bufferBitmap, cantidadBloques/8, MSB_FIRST);
	
	free(bufferBitmap);*/
	free(buffer);
	fclose(archivoSuperbloque);
}

void subirBlocksAMemoria(){
	int archivoBlocks = open(pathBlocks, O_RDWR);
	int tamanioBlocks = cantidadBloques * tamanioBloque;
	
	punteroBlocks = mmap(NULL, tamanioBlocks, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBlocks, 0);
	miLogInfo("Se subió el archivo Blocks.ims a memoria.");

	close(archivoBlocks);
}

int buscarYAsignarProximoBloqueLibre(void){
//Ejemplo de uso: int bloqueLibre = buscarYAsignarProximoBloqueLibre(configuracion->puntoMontaje);

	pthread_mutex_lock(&mutex_bitmap);
	for(int i=0; i<cantidadBloques; i++){
		if(bitarray_test_bit(bitmap, i) == 0){
			bitarray_set_bit(bitmap ,i); //Asigno el bloque libre (pongo en 1 el bit).
			msync(bitmap, cantidadBloques/8, 0); //Fuerzo la actualización del bitmap en el archivo.

			miLogDebug("Asignó el bloque %d.", i);	
			pthread_mutex_unlock(&mutex_bitmap);
			return i; //Devuelvo el número de bloque que asigné.		 
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);

	/*bitarray_destroy(bitmap);
	close(archivoSuperbloque);
	free(pathSuperbloque);

	if (munmap(punteroSuperbloque, tamanioSuperbloque) == -1){ 
		perror("Falló al desmapear el Superbloque.");
        exit(1); 
    }*/
	
	miLogError("No pudo asignar ningun bloque. El Bitmap estaba lleno!.");
	return -1;
}

void liberarBloque(int index){
//Ejemplo de uso: liberarBloque(nroBloque);

	pthread_mutex_lock(&mutex_bitmap);
	
	bitarray_clean_bit(bitmap, index);
	miLogInfo("Se liberó el bloque %d", index );
	msync(bitmap, cantidadBloques/8 ,0);
	
	pthread_mutex_unlock(&mutex_bitmap);
}


int calcularCantBloques(int size) {

	int cantidadBloques = size / tamanioBloque;
	int resto = size % (tamanioBloque); //Para saber si esta completo.
	
	if(resto != 0){
		cantidadBloques++; // No estaba completo -> agrego un bloque mas para el bloque incompleto.
	}

	return cantidadBloques;
}

// Recibo un bloque incompleto y lo completo.
void escribirBloqueUsado(int bloque, int cantidadBytesLibres, char* datos){

	escribirBloque(bloque, tamanioBloque - cantidadBytesLibres, datos);
}

// Recibo la cadena de caracteres que tengo que escribir. Busco bloques libres y los voy escribiendo.
// Devuelvo la lista de bloques que ocupe.
t_list* escribirBloquesNuevo(char* datos){ 
	
	int cantidadBloquesNecesarios = calcularCantBloques(string_length(datos));
	int startEscritura = 0;
	
	t_list* bloques = list_create();	

	for(int i = 0; i < cantidadBloquesNecesarios; i++) {
		int bloqueNuevo = buscarYAsignarProximoBloqueLibre();
		if(bloqueNuevo == -1) {
			miLogError("No hay mas lugar en el archivo Blocks.ims");
			return -1;
		}
		
		int tamanioEscritura = tamanioBloque;

		if(i + 1 == cantidadBloquesNecesarios){ // Tratamiento del último bloque.
			int diferencialBloque = string_length(datos) % (tamanioBloque); // Me fijo si ocupa un bloque completo.

			if(diferencialBloque != 0){
				tamanioEscritura = diferencialBloque; // No ocupa un bloque entero -> lo que escribo es lo que ocupa.
			}
		}
		
		char* escritura = string_substring(datos, startEscritura, tamanioEscritura);

		escribirBloque(bloqueNuevo, 0, escritura);
		list_add(bloques, (void*)bloqueNuevo);
		startEscritura += tamanioBloque;	
		free(escritura);
	}
	//list_destroy(bloques);
	return bloques;
}


void escribirBloque(int bloque, int offset, char* escritura){
	
	//|----------|----------|----------|----------|----------|

	int desplazamiento = (bloque * tamanioBloque) + offset;

	pthread_mutex_lock(&mutex_bloques);
	memcpy(punteroBlocks + desplazamiento, escritura, string_length(escritura));
	pthread_mutex_unlock(&mutex_bloques);
}

char* leerBloques(t_list* bloques, int size){
	char* lectura = string_new();
	char* lecturaAux;
	int cantidadLecturas = calcularCantBloques(size);
	int proximoBloque = list_get(bloques, 0);

	for(int i = 0; i < cantidadLecturas - 1; i++){ //Hasta cantidadLecturas - 1 para analizar el ultimo bloque despues.
		lecturaAux = leerBloque(proximoBloque, tamanioBloque);
		string_append(&lectura, lecturaAux);
		proximoBloque = list_get(bloques, i + 1);
		free(lecturaAux);
	}
	
	int resto = size % (tamanioBloque); //Para saber si esta completo.
	if(resto == 0){
		resto = tamanioBloque; //Estaba completo -> lo que falta es un bloque entero.
	}
	lecturaAux = leerBloque(proximoBloque, resto); //Ultimo bloque.
	string_append(&lectura, lecturaAux);
	
	free(lecturaAux);
	return lectura;
}

char* leerBloque(int bloque, int size) {
	char* lectura = malloc(tamanioBloque);
	int desplazamiento = bloque * tamanioBloque;

	pthread_mutex_lock(&mutex_bloques);
	memcpy(lectura, punteroBlocks + desplazamiento, tamanioBloque);
	
	miLogDebug("Leyó: %s, del bloque: %d", lectura, bloque);
	lectura = string_substring(lectura, 0, size); // Porque tengo que hacer esto???

	pthread_mutex_unlock(&mutex_bloques);

	return lectura;
}

int tamanioArchivo(char* path){

	FILE* fd = fopen(path, "rb");
	fseek(fd, 0, SEEK_END);
	int tamanio = ftell(fd);
	fclose(fd);

	return tamanio;
}


void finalizarFS(void){
	
	bitarray_destroy(bitmap);

	/*if (munmap(punteroSuperbloque, tamanioSuperbloque) == -1){ 
		perror("Falló al desmapear el Superbloque.");
        exit(1); 
    }*/
	if (munmap(punteroBlocks, tamanioBloque * cantidadBloques) == -1){ 
		perror("Falló al desmapear el Superbloque.");
        exit(1); 
    }

	free(puntoMontaje);    
	free(pathSuperbloque); 
	free(pathBlocks);  
	free(pathFiles);
	free(pathBitacoras);

}

//------------------------------------MANEJO DE METADATA----------------------------------
MetadataRecurso* leerMetadataRecurso(tipoRecurso recurso){

	char * direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);	

	if(verificarExistenciaFile(direccionDeMetadata)){
		if(crearMetadataRecurso(recurso)){
			free(direccionDeMetadata);
			return NULL;
		}		
	}

	t_config* metaConfig;
	MetadataRecurso* metadata = malloc(sizeof(MetadataRecurso));

	metaConfig = config_create(direccionDeMetadata);

	metadata->size = config_get_int_value(metaConfig, "SIZE");
	metadata->block_count = config_get_int_value(metaConfig, "BLOCK_COUNT");
	metadata->blocks = listaFromArray(config_get_array_value(metaConfig, "BLOCKS"));
	metadata->caracter_llenado = config_get_string_value(metaConfig, "CARACTER_LLENADO");
	metadata->md5 = config_get_string_value(metaConfig, "MD5");

	config_destroy(metaConfig);
	free(direccionDeMetadata);

	return metadata;
}

MetadataBitacora* leerMetadataBitacora(char* tripulante){

	char * direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);	

	if(verificarExistenciaFile(direccionDeMetadata)){
		if(crearMetadataBitacora(tripulante)){
			free(direccionDeMetadata);
			return NULL;
		}		
	}

	t_config* metaConfig;
	MetadataBitacora* metadata = malloc(sizeof(MetadataBitacora));

	metaConfig = config_create(direccionDeMetadata);

	metadata->size = config_get_int_value(metaConfig, "SIZE");
	metadata->block_count = config_get_int_value(metaConfig, "BLOCK_COUNT");
	metadata->blocks = listaFromArray(config_get_array_value(metaConfig, "BLOCKS"));

	config_destroy(metaConfig);
	free(direccionDeMetadata);

	return metadata;
}

int modificarMetadataRecurso(MetadataRecurso* metadata, tipoRecurso recurso){

	t_config* metaConfig;
	char * direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);
	
	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig==NULL){
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}

	config_set_value(metaConfig, "SIZE", string_itoa(metadata->size));
	config_set_value(metaConfig, "BLOCK_COUNT", string_itoa(metadata->block_count));
	config_set_value(metaConfig, "BLOCKS", stringFromList(metadata->blocks));
	config_set_value(metaConfig, "CARACTER_LLENADO", metadata->caracter_llenado);
	config_set_value(metaConfig, "MD5_ARCHIVO", generarMd5(metadata->blocks));

	config_save(metaConfig);
	config_destroy(metaConfig);
	
	list_destroy(metadata->blocks);
	free(metadata);
	free(direccionDeMetadata);

	return EXIT_SUCCESS;
}

int modificarMetadataBitacora(MetadataBitacora* metadata, char* tripulante){

	t_config* metaConfig;
	char * direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);	

	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig==NULL){
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}

	config_set_value(metaConfig, "SIZE", string_itoa(metadata->size));
	config_set_value(metaConfig, "BLOCK_COUNT", string_itoa(metadata->block_count));
	config_set_value(metaConfig, "BLOCKS", stringFromList(metadata->blocks));

	config_save(metaConfig);
	config_destroy(metaConfig);
	
	list_destroy(metadata->blocks);
	free(metadata);
	free(direccionDeMetadata);

	return EXIT_SUCCESS;
}

int crearMetadataRecurso(tipoRecurso recurso){
	
	char * direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);
	FILE* file = fopen(direccionDeMetadata, "w+");
	
	if (file == NULL) {
		free(direccionDeMetadata);
		return EXIT_FAILURE;
	}
	fclose(file);
	
	t_config* metaConfig;
	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig == NULL){
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}

	char caracter = cualEsMiCaracter(recurso);
	char* strCaracter = string_new();
	strCaracter = string_repeat(caracter, 1);
	
	config_set_value(metaConfig, "SIZE", "0");
	config_set_value(metaConfig, "BLOCK_COUNT", "0");
	config_set_value(metaConfig, "BLOCKS", "[]");
	config_set_value(metaConfig, "CARACTER_LLENADO", strCaracter);
	config_set_value(metaConfig, "MD5_ARCHIVO", "");

	config_save(metaConfig);
	config_destroy(metaConfig);
	free(direccionDeMetadata);

	return EXIT_SUCCESS;
}

int crearMetadataBitacora(char* tripulante){
	
	char * direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);
	FILE* file = fopen(direccionDeMetadata, "w+");
	
	if (file == NULL) {
		free(direccionDeMetadata);
		return EXIT_FAILURE;
	}
	fclose(file);
	
	t_config* metaConfig;
	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig == NULL){
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}
	
	config_set_value(metaConfig, "SIZE", "0");
	config_set_value(metaConfig, "BLOCK_COUNT", "0");
	config_set_value(metaConfig, "BLOCKS", "[]");

	config_save(metaConfig);
	config_destroy(metaConfig);
	free(direccionDeMetadata);

	return EXIT_SUCCESS;
}

//TODO:
char* generarMd5(t_list* bloques){
	return "Algun md5"; 
}

char* obtenerDireccionDeMetadataRecurso (tipoRecurso recurso){ //Devuelve la direccion de la metadata según el recurso que quiero

	char* direccionDeMetadata = string_new();

	switch(recurso)
	{
		case OXIGENO:
			direccionDeMetadata = string_from_format("%s/%s", pathFiles, "Oxigeno.ims");
			break;
		case COMIDA:
			direccionDeMetadata = string_from_format("%s/%s", pathFiles, "Comida.ims");
			break;
		case BASURA:
			direccionDeMetadata = string_from_format("%s/%s", pathFiles, "Basura.ims");
			break;
		default:
			miLogInfo("Error: No se reconoce el recurso.\n");
			direccionDeMetadata = NULL;
			break;
	}

	return direccionDeMetadata;
}

char* obtenerDireccionDeMetadataBitacora (char* tripulante){ //Devuelve la direccion de la metadata según el id del tripulante que quiero

	char* direccionDeMetadata = string_new();
	char* nombreMetadata = string_from_format("%s/%s", pathBitacoras, "Tripulante");
	
	string_append(&direccionDeMetadata, nombreMetadata);
	string_append(&direccionDeMetadata, tripulante);
	string_append(&direccionDeMetadata, ".ims");

	free(nombreMetadata);
	
	return direccionDeMetadata;
}

int verificarExistenciaFile(char* path){
	
	FILE* file = fopen(path, "r");
	
	if (file == NULL) {
			return EXIT_FAILURE;
	}
	fclose(file);

	return EXIT_SUCCESS;
}

t_list* listaFromArray(char** array){
	t_list* listaBloques = list_create();

	for(int i = 0; array[i] != NULL; i++){
		int bloque = atoi(array[i]);
		list_add(listaBloques,(void*) bloque);
	}
	
	return listaBloques;
}

char* stringFromList(t_list* lista){
	char* strLista = string_new();
	string_append(&strLista, "[");
	
	if(string_length(lista) > 0){
		string_append(&strLista, string_itoa(list_get(lista, 0)));
		
		for(int i=1; i<list_size(lista);i++){
			string_append(&strLista, ",");
			string_append(&strLista, string_itoa(list_get(lista, i)));	
		}
	}

	string_append(&strLista, "]");
	return strLista;
}

/*char* stringFromRecurso(tipoRecurso f){} //Obtiene un string de acuerdo al enum que le envío

    static const char *strings[] = { "oxigeno", "comida", "basura"};
    return strings[f];
}*/
