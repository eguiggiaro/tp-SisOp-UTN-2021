#include "file_system.h"


bool verificarFS(void){
		return !(fopen(pathSuperbloque, "r") == NULL || fopen(pathBlocks, "r") == NULL);
}

void crearDirectorio(char* path){

	if(mkdir(path, 0777) == -1){
		printf("No se pudo crear el directorio %s.\n", path);
		exit(1);
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
	
	FILE* archivoSuperbloque = fopen(pathSuperbloque, "w+");
	if(archivoSuperbloque==NULL){
		miLogInfo("No se pudo crear el archivo de Superbloque.ims.");
		exit(1);
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
		exit(1);
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

	close(archivoBlocks);
}

int buscarYAsignarProximoBloqueLibre(void){
//Ejemplo de uso: int bloqueLibre = buscarYAsignarProximoBloqueLibre(configuracion->puntoMontaje);

	for(int i=0; i<cantidadBloques; i++){
		if(bitarray_test_bit(bitmap, i) == 0){
			bitarray_set_bit(bitmap ,i); //Asigno el bloque libre (pongo en 1 el bit).
			msync(bitmap, cantidadBloques/8, 0); //Fuerzo la actualización del bitmap en el archivo.

			miLogInfo("Asignó el bloque %d.", i);	
			return i; //Devuelvo el número de bloque que asigné.		 
		}
	}
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
	bitarray_clean_bit(bitmap, index);
	miLogInfo("Se liberó el bloque %d", index );

	msync(bitmap, cantidadBloques/8 ,0);
}


int calcularCantBloques(char* datos) {

	int cantidadBloques = string_length(datos) / tamanioBloque;
	int resto = string_length(datos) % (tamanioBloque); //Para saber si esta completo.
	
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
	
	int cantidadBloquesNecesarios = calcularCantBloques(datos);
	int startEscritura = 0;
	int endEscritura; 

	t_list* bloques = list_create();	

	for(int i = 0; i < cantidadBloquesNecesarios; i++) {
		int bloqueNuevo = buscarYAsignarProximoBloqueLibre();
		if(bloqueNuevo == -1) {
			printf("No hay mas lugar en el archivo Blocks.ims");
			return -1;
		}
		
		endEscritura = startEscritura + tamanioBloque;

		if(i + 1 == cantidadBloquesNecesarios){
			endEscritura = string_length(datos); //El ultimo bloque tengo que ir hasta el final de la cadena.
		}
		//endEscritura = string_lenght(datos) * X + (startEscritura + tamanioBloque) * Y;
		
		escribirBloque(bloqueNuevo, 0, string_substring(datos, startEscritura, endEscritura));
		list_add(bloques, (void*)bloqueNuevo);
		startEscritura = endEscritura + 1;	
	}
	//list_destroy(bloques);
	return bloques;
}


void escribirBloque(int bloque, int offset, char* escritura){
	
	int desplazamiento = (bloque - 1) * tamanioBloque + offset;
	memcpy(punteroBlocks + desplazamiento, escritura, string_length(escritura));
}

//TODO: 
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
	char* lectura = malloc(size);
	int desplazamiento = (bloque - 1) * tamanioBloque;

	memcpy(lectura, punteroBlocks + desplazamiento, size);

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
	free(puntoMontaje);    
	free(pathSuperbloque); 
	free(pathBlocks);  
	free(pathFiles);
	free(pathBitacoras);

}

