#include "file_system.h"
#include "ejecucion_tareas.h"
#include "md5.h"

/********** MANEJO FILE SYSTEM **********/
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

void crearSuperbloque(void){
	
	FILE* archivoSuperbloque = fopen(pathSuperbloque, "wb+");
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
	
	FILE* archivoBlocks = fopen(pathBlocks, "w+");	//TODO: Pensar, es necesario que sea binario?
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

	punteroSuperbloque = mmap(NULL, tamanioSuperbloque, PROT_READ | PROT_WRITE, MAP_SHARED, archivoSuperbloque, 0);
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

void subirBlocksAMemoria(){
	int archivoBlocks = open(pathBlocks, O_RDWR);
	int tamanioBlocks = cantidadBloques * tamanioBloque;
	
	punteroBlocks = mmap(NULL, tamanioBlocks, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBlocks, 0);
	miLogInfo("Se subió el archivo Blocks.ims a memoria.");

	close(archivoBlocks);
}

void finalizarFS(void){
	
	bitarray_destroy(bitmap);
	if (munmap(punteroBlocks, tamanioBloque * cantidadBloques) == -1){ 
		perror("Falló al desmapear el Superbloque.");
        exit(1); 
    }

	pthread_mutex_destroy(&mutex_bitmap);
	pthread_mutex_destroy(&mutex_bloques);
	pthread_mutex_destroy(&mutex_escribirBloques);
	
	free(puntoMontaje);    
	free(pathSuperbloque); 
	free(pathBlocks);  
	free(pathFiles);
	free(pathBitacoras);
}

/********** MANEJO BLOQUES **********/
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

int verificarQueTengoLosBloquesNecesarios(int cantidadBloquesNecesarios){
	int resultado;
	int bloquesEncontrados = 0;

	pthread_mutex_lock(&mutex_bitmap);
	for(int i=0; i<cantidadBloques; i++){
		if(bitarray_test_bit(bitmap, i) == 0){
			bloquesEncontrados++;
		}
		if(bloquesEncontrados == cantidadBloquesNecesarios){
			pthread_mutex_unlock(&mutex_bitmap);
			return 1;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);

	return 0;
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
	
	pthread_mutex_lock(&mutex_escribirBloques);

	//Antes de empezar tengo que verificar si tengo lugar para todos los bloques. 
	if(!verificarQueTengoLosBloquesNecesarios(cantidadBloquesNecesarios)){
		miLogError("No hay mas lugar en el archivo Blocks.ims");
		return -1;
	}
	
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
	
	pthread_mutex_unlock(&mutex_escribirBloques);
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

//------------------------------------MANEJO DE METADATA----------------------------------
MetadataRecurso* leerMetadataRecurso(tipoRecurso recurso){

	char* direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);	

	if(verificarExistenciaFile(direccionDeMetadata)){
		if(crearMetadataRecurso(recurso)){
			free(direccionDeMetadata);
			return NULL;
		}		
	}

	t_config* metaConfig = malloc(sizeof(MetadataRecurso));
	MetadataRecurso* metadata = malloc(sizeof(MetadataRecurso));

	metaConfig = config_create(direccionDeMetadata);

	metadata->size = config_get_int_value(metaConfig, "SIZE");
	metadata->block_count = config_get_int_value(metaConfig, "BLOCK_COUNT");
	metadata->blocks = listaFromArray(config_get_array_value(metaConfig, "BLOCKS"));
	metadata->caracter_llenado = *config_get_string_value(metaConfig, "CARACTER_LLENADO");
	strcpy(metadata->md5, config_get_string_value(metaConfig, "MD5_ARCHIVO"));

	config_destroy(metaConfig);
	free(direccionDeMetadata);

	return metadata;
}

MetadataBitacora* leerMetadataBitacora(char* tripulante){

	char* direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);	

	if(verificarExistenciaFile(direccionDeMetadata)){
		if(crearMetadataBitacora(tripulante)){
			free(direccionDeMetadata);
			return NULL;
		}		
	}

	t_config* metaConfig = malloc(sizeof(MetadataBitacora));
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

	t_config* metaConfig = malloc(sizeof(MetadataRecurso));
	char* direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);
	
	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig==NULL){
		freeMetadataRecurso(metadata);
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}

	char* stringBloques = stringFromList(metadata->blocks);

	config_set_value(metaConfig, "SIZE", string_itoa(metadata->size));
	config_set_value(metaConfig, "BLOCK_COUNT", string_itoa(metadata->block_count));
	config_set_value(metaConfig, "BLOCKS", stringBloques);
	config_set_value(metaConfig, "CARACTER_LLENADO", string_substring_until(&metadata->caracter_llenado,1));
	config_set_value(metaConfig, "MD5_ARCHIVO", generarMd5(metadata->blocks, metadata->size));

	config_save(metaConfig);
	config_destroy(metaConfig);
	
	freeMetadataRecurso(metadata);
	free(direccionDeMetadata);
	free(stringBloques);

	return EXIT_SUCCESS;
}

int modificarMetadataBitacora(MetadataBitacora* metadata, char* tripulante){

	t_config* metaConfig = malloc(sizeof(MetadataBitacora));
	char* direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);	

	metaConfig = config_create(direccionDeMetadata);

	if(metaConfig==NULL){
		freeMetadataBitacora(metadata);
		free(direccionDeMetadata);
		return EXIT_FAILURE;	
	}
	char* stringBloques = stringFromList(metadata->blocks);

	config_set_value(metaConfig, "SIZE", string_itoa(metadata->size));
	config_set_value(metaConfig, "BLOCK_COUNT", string_itoa(metadata->block_count));
	config_set_value(metaConfig, "BLOCKS", stringBloques);

	config_save(metaConfig);
	config_destroy(metaConfig);
	
	freeMetadataBitacora(metadata);
	free(stringBloques);
	free(direccionDeMetadata);

	return EXIT_SUCCESS;
}

int crearMetadataRecurso(tipoRecurso recurso){
	
	char* direccionDeMetadata = obtenerDireccionDeMetadataRecurso(recurso);
	FILE* file = fopen(direccionDeMetadata, "w+");
	
	if (file == NULL) {
		free(direccionDeMetadata);
		return EXIT_FAILURE;
	}
	fclose(file);
	
	t_config* metaConfig = malloc(sizeof(MetadataRecurso));
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
	
	char* direccionDeMetadata = obtenerDireccionDeMetadataBitacora(tripulante);
	FILE* file = fopen(direccionDeMetadata, "w+");
	
	if (file == NULL) {
		free(direccionDeMetadata);
		return EXIT_FAILURE;
	}
	fclose(file);
	
	t_config* metaConfig = malloc(sizeof(MetadataBitacora));
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

void freeMetadataRecurso(MetadataRecurso* metadata){
	
	list_destroy(metadata->blocks);
	free(metadata);
}

void freeMetadataBitacora(MetadataBitacora* metadata){
	
	list_destroy(metadata->blocks);
	free(metadata);
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

int eliminarArchivoBasura(){

	char* archivoBasura = string_from_format("%s/%s", pathFiles, "Basura.ims");

	FILE* basura = fopen(archivoBasura, "r");

	if (basura == NULL) {
		free(archivoBasura);
		miLogError("No se pudo borrar el archivo Basura.ims, ya que el archivo NO EXISTE.");	
		return EXIT_FAILURE;
	}
	fclose(basura);

	if (!remove(archivoBasura)){
		miLogInfo("Se borró el archivo Basura.ims.");
		return EXIT_SUCCESS;
	} else {
		miLogError("No se pudo borrar el archivo Basura.ims.");	
		return EXIT_FAILURE;	
	}	
}

/********** PROTOCOLO SABOTAJE **********/
int verificarCantidadBloques(){
	//Se debe garantizar que la cantidad de bloques que hay en el sistema sea igual a la cantidad que dice haber en el superbloque. 
	int tamanioBlocksActual = tamanioArchivo(pathBlocks);

	if(tamanioBlocksActual != tamanioBloque * cantidadBloques){
		if(repararCantidadBloques(tamanioBlocksActual)){
			miLogError("No pudo reparar el Superbloque por error en cantiudad de bloques.");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int repararCantidadBloques(int tamanioBlocksActual){
	//Reparación: sobreescribir “cantidad de bloques” del superbloque con la cantidad de bloques real en el disco.
	uint32_t cantidadBloquesActual = tamanioBlocksActual / tamanioBloque;
	int resto = tamanioBlocksActual % tamanioBloque; //Para saber si esta completo.
	
	if(resto != 0){
		cantidadBloquesActual++; // No estaba completo -> agrego un bloque mas para el bloque incompleto.
	}
	//Actualizando el puntero.
	int tamanioSuperbloqueActual = cantidadBloquesActual/8 + sizeof(uint32_t) * 2;
	
	memcpy(punteroSuperbloque + sizeof(uint32_t), &cantidadBloquesActual, sizeof(uint32_t));
	msync(punteroSuperbloque, tamanioSuperbloqueActual, 0);

	cantidadBloques = cantidadBloquesActual;

	//Actualizando directamente el archivo.
	/*FILE* archivoSuperbloque = fopen(pathSuperbloque, "rb+");
	if(archivoSuperbloque==NULL){
		miLogError("No se pudo abrir el archivo de Superbloque.ims para actualizar la cantidad de bloques.");
		exit(EXIT_FAILURE);
	}

    fseek(archivoSuperbloque, sizeof(uint32_t), SEEK_SET);
 	fwrite(&cantidadBloquesActual, 1 , sizeof(uint32_t), archivoSuperbloque);			
	fclose(archivoSuperbloque);
	*/
	return EXIT_SUCCESS;
}

int verificarBitmap(){
	//Se debe garantizar que los bloques libres y ocupados marcados en el bitmap sean los que dicen ser. 
	t_list* bloquesOcupados = list_create();
	int cantidadArchivosBitacoras = cantidadArchivosEnDirectorio(pathBitacoras);
	
	MetadataBitacora* metadata = malloc(sizeof(MetadataBitacora));
	//Levanto todos los bloques en uso de las metadatas de bitacoras.
	for(int i=0; i<cantidadArchivosBitacoras; i++){
		char* numeroTripulante = string_itoa(i);
		metadata = leerMetadataBitacora(numeroTripulante);
		list_add_all(bloquesOcupados, metadata->blocks);
	}
	
	t_list* recursos = verificarQueArchivosDeRecursosHay();

	MetadataRecurso* metadataR = malloc(sizeof(MetadataRecurso));
	//Levanto todos los bloques en uso de las metadatas de recursos.
	for(int i=0; i<list_size(recursos); i++){
		metadataR = leerMetadataRecurso((tipoRecurso)list_get(recursos,i));
		list_add_all(bloquesOcupados, metadataR->blocks);
	}

	if (repararBitmap(bloquesOcupados)){
		miLogError("No pudo reparar el bitmap del Superbloque.");
		list_destroy(bloquesOcupados);
		return EXIT_FAILURE;
	}

	list_destroy(bloquesOcupados);
	return EXIT_SUCCESS;
}

int repararBitmap(t_list* bloques){
	//Reparación: corregir el bitmap con lo que se halle en la metadata de los archivos.
	//char* lista = stringFromList(bloques);	

	pthread_mutex_lock(&mutex_bitmap);
	//Primero recorro toda la lista de bloques ocupados según los metadata, y si no estan en 1 los pongo en 1 (ocupados).
	for(int i=0; i<list_size(bloques); i++){
		int bloque = list_get(bloques, i);

		if(bitarray_test_bit(bitmap, bloque) == 0){
			bitarray_set_bit(bitmap, bloque); 		 //Marco el bloque como usado si no estaba en 1.
			msync(bitmap, cantidadBloques/8, 0); //Fuerzo la actualización del bitmap en el archivo.

			miLogWarning("Seteo el bloque %d en 1, porque hubo un sabotaje en ese bloque.", bloque);				
		} 
	}
	//Despues recorro todos los otros bloques y corroboro que esten en 0 (libres).
	for(int i=0; i<cantidadBloques; i++){
		 int _estaEnLaLista(int b) {
            return b == i;
        }

		if(!list_any_satisfy(bloques, (void*) _estaEnLaLista)){					
			if(bitarray_test_bit(bitmap, i) == 1){
				bitarray_clean_bit(bitmap, i); 		 //Marco el bloque como libre si no estaba en 0.
				msync(bitmap, cantidadBloques/8, 0); //Fuerzo la actualización del bitmap en el archivo.
			
				miLogWarning("Seteo el bloque %d en 0, porque hubo un sabotaje en ese bloque.", i);						
			}
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);

	return EXIT_SUCCESS;
}

int verificarSizeEnFile(){
	//Se debe asegurar que el tamaño del archivo sea el correcto, validando con el contenido de sus bloques. 
	t_list* recursos = verificarQueArchivosDeRecursosHay();

	MetadataRecurso* metadataR = malloc(sizeof(MetadataRecurso));
	//Levanto todos los bloques en uso de las metadatas de recursos.
	for(int i=0; i<list_size(recursos); i++){
		tipoRecurso recurso = (tipoRecurso)list_get(recursos,i);
		metadataR = leerMetadataRecurso(recurso);
		int ultimoBloque = list_get(metadataR->blocks,metadataR->block_count - 1);
		int tamanioRealArchivo = (metadataR->block_count - 1) * tamanioBloque + tamanioOcupadoRecursoEnBloque(ultimoBloque, recurso);
		if(metadataR->size != tamanioRealArchivo){
			if(repararSizeEnFile(metadataR, recurso, tamanioRealArchivo)){
				miLogError("No pudo reparar el size del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.
				list_destroy(recursos);
				return EXIT_FAILURE;
			}
		}
	}

	list_destroy(recursos);
	return EXIT_SUCCESS;
}

int repararSizeEnFile(MetadataRecurso* metadata, tipoRecurso recurso, int tamanioReal){
	//Reparación: Asumir correcto lo encontrado en los bloques.
	metadata->size = tamanioReal;
	if(modificarMetadataRecurso(metadata, recurso)){
		miLogError("No pudo actualizar la metadata del archivo al corregir el size del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int verificarBlockCount(){
	//Al encontrar una diferencia entre estos dos se debe restaurar la consistencia. 
	t_list* recursos = verificarQueArchivosDeRecursosHay();

	MetadataRecurso* metadataR = malloc(sizeof(MetadataRecurso));
	//Levanto todos los bloques en uso de las metadatas de recursos.
	for(int i=0; i<list_size(recursos); i++){
		tipoRecurso recurso = (tipoRecurso)list_get(recursos,i);
		metadataR = leerMetadataRecurso(recurso);
		if(metadataR->block_count != list_size(metadataR->blocks)){
			if(repararBlockCount(metadataR, recurso)){
				miLogError("No pudo reparar el block count del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.		
				list_destroy(recursos);				
				return EXIT_FAILURE;
			}
		}
	}

	list_destroy(recursos);	
	return EXIT_SUCCESS;
}

int repararBlockCount(MetadataRecurso* metadata, tipoRecurso recurso){
	//Reparación: Actualizar el valor de Block_count en base a lo que está en la lista de Blocks.
	metadata->block_count = list_size(metadata->blocks);

	if(modificarMetadataRecurso(metadata, recurso)){
		miLogError("No pudo actualizar la metadata del archivo al corregir el size del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int verificarBlocks(){
	//Se debe validar que los números de bloques en la lista sean válidos dentro del FS. 
	t_list* recursos = verificarQueArchivosDeRecursosHay();

	MetadataRecurso* metadataR = malloc(sizeof(MetadataRecurso));
	//Levanto todos los bloques en uso de las metadatas de recursos.
	for(int i=0; i<list_size(recursos); i++){
		tipoRecurso recurso = (tipoRecurso)list_get(recursos,i);
		metadataR = leerMetadataRecurso(recurso);
		if(!compararMd5(metadataR)){
			if(repararBlocks(metadataR, recurso)){
				miLogError("No pudo reparar los bloques del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.
				list_destroy(recursos);				
				return EXIT_FAILURE;
			}
		}
	}

	list_destroy(recursos);
	return EXIT_SUCCESS;
}

int repararBlocks(MetadataRecurso* metadata, tipoRecurso recurso){
	//Reparación: Restaurar archivo -> Tomar como referencia el size del archivo y el caracter de llenado e ir llenando los bloques 
	//hasta completar el size del archivo. En caso de que falten bloques, los mismos se deberán agregar al final del mismo.
	char caracter = metadata->caracter_llenado;
	int cantidadBytesAEscribir = metadata->size;
	char* escrituraCompleta = string_repeat(caracter, tamanioBloque);

	for(int i=0; i<list_size(metadata->blocks);i++){
		int bloque = list_get(metadata->blocks, i);
		if(cantidadBytesAEscribir >= tamanioBloque){
			escribirBloque(bloque, 0, escrituraCompleta);
			cantidadBytesAEscribir -= tamanioBloque;
		} else {
			escribirBloque(bloque, 0, string_substring_until(escrituraCompleta, cantidadBytesAEscribir));
			cantidadBytesAEscribir = 0;
		}
	}

	//Si todavia quedan caracteres para escribir, los escribo en bloques nuevos y actualizo la metadata del recurso.
	if(cantidadBytesAEscribir > 0){
		t_list* nuevosBloques = list_create();
		nuevosBloques = escribirBloquesNuevo(string_substring_from(escrituraCompleta, cantidadBytesAEscribir));
		list_add_all(metadata->blocks, nuevosBloques);

		list_destroy(nuevosBloques);
	}

	//msync(punteroBlocks, cantidadBloques*tamanioBloque, 0);
	if(modificarMetadataRecurso(metadata, recurso)){
		miLogError("No pudo actualizar la metadata del archivo al corregir la lista de bloques del archivo."); //TODO: ver si se ṕuede poner el nombre del recurso.
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


/********** SOPORTE **********/
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
	liberar_lista(array);
	return listaBloques;
}

void liberar_lista(char** lista){
	int contador = 0;
	while(lista[contador] != NULL){
			free(lista[contador]);
			contador++;
	}

	free(lista);
}

char* stringFromList(t_list* lista){
	char* strLista = string_new();
	char* bloque;

	string_append(&strLista, "[");
	
	if(list_size(lista) > 0){
		bloque = string_itoa(list_get(lista, 0));
		string_append(&strLista, bloque);
		free(bloque);

		for(int i=1; i<list_size(lista);i++){
			bloque = string_itoa(list_get(lista, i));
			string_append(&strLista, ",");			
			string_append(&strLista, bloque);	
			free(bloque);
		}
	}
	string_append(&strLista, "]");
	
	return strLista;
}

char* generarMd5(t_list* bloques, int size){

	char* strMd5 = string_new();
	unsigned char hash[16];

	//char* strBloques = stringFromList(bloques);
	if(list_size(bloques) == 0){
		return "";
	}
	
	char* lectura = leerBloques(bloques, size);

	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, lectura, string_length(lectura));
	MD5Final(&md5, hash);
	
	for(int i=0; i<16; i++){
		//printf("%02x",hash[i]);
		string_append_with_format(&strMd5, "%02x", hash[i]);
	}
	//printf(strMd5);
	string_to_upper(strMd5);

	free(lectura);
	return strMd5;
}

int tamanioArchivo(char* path){

	FILE* fd = fopen(path, "rb");
	fseek(fd, 0, SEEK_END);
	int tamanio = ftell(fd);
	fclose(fd);

	return tamanio;
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

int cantidadArchivosEnDirectorio(char* path){
	int cantidad = 0;
	
	DIR * directorio;
	struct dirent * entry;

	directorio = opendir(path); 

	while ((entry = readdir(directorio)) != NULL) {
		if (entry->d_type == DT_REG) { /* If the entry is a regular file */
				cantidad++;
		}
	}
	closedir(directorio);

	return cantidad;
}

t_list* verificarQueArchivosDeRecursosHay(){

	char* direccionDeMetadataOxigeno = string_from_format("%s/%s", pathFiles, "Oxigeno.ims");
	char* direccionDeMetadataComida = string_from_format("%s/%s", pathFiles, "Comida.ims");
	char* direccionDeMetadataBasura = string_from_format("%s/%s", pathFiles, "Basura.ims");

	t_list* recursos = list_create();

	FILE* fOxigeno = fopen(direccionDeMetadataOxigeno, "r");
	if(fOxigeno!=NULL){
		list_add(recursos, OXIGENO);
	}
	FILE* fComida = fopen(direccionDeMetadataComida, "r");
	if(fComida!=NULL){
		list_add(recursos, COMIDA);
	}
	FILE* fBasura = fopen(direccionDeMetadataBasura, "r");
	if(fBasura!=NULL){
		list_add(recursos, BASURA);
	}

	return recursos;

}

int tamanioOcupadoRecursoEnBloque(int bloque, tipoRecurso recurso){
	int posicionUltimoCaracter = 0;
	char caracter = cualEsMiCaracter(recurso);

	char* contenidoUltimoBloque = leerBloque(bloque, tamanioBloque);
	int tamanioContenido = string_length(contenidoUltimoBloque);

	for(int i=0; i<tamanioBloque; i++){
		if(contenidoUltimoBloque[i] != caracter){
			posicionUltimoCaracter = i;
			break;
		}
	}

	return posicionUltimoCaracter;
}

bool compararMd5(MetadataRecurso* metadata){

	char* md5SegunBloques = generarMd5(metadata->blocks, metadata->size);
	
	return string_equals_ignore_case(md5SegunBloques, metadata->md5);
}

