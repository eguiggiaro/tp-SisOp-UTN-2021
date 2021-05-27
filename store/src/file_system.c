#include "file_system.h"


bool verificarFS(void){
		return !(fopen(pathSuperbloque, "r") == NULL || fopen(pathBlocks, "r") == NULL);
}

bool verificarEstructuraDirectorios(void){
	DIR* dirMontaje = opendir(puntoMontaje);
	DIR* dirFiles = opendir(pathFiles);
	DIR* dirBitacoras = opendir(pathBitacoras);
	bool respuesta;

	if(dirMontaje) {
		respuesta = true;
	} else {
		if(dirFiles){
			respuesta = true;
		} else {
			if(dirBitacoras){
				respuesta = true;
			}
		}
		respuesta = false;
	}

	closedir(dirMontaje);
	closedir(dirFiles);
	closedir(dirBitacoras);

	return respuesta;
}

void crearArbolDirectorios(void){

	if(mkdir(puntoMontaje, 0777) == -1){
		printf("No se pudo crear el punto de montaje.\n");
		exit(1);
	}
	miLogInfo("Agrego directorio punto de montaje.");

	if(mkdir(pathFiles, 0777) == -1){
		printf("No se pudo crear el directorio Files.\n");
		exit(1);
	}
	miLogInfo("Agrego directorio Files");
	
	if(mkdir(pathBitacoras, 0777) == -1){
		printf("No se pudo crear el directorio Bitácoras.\n");
		exit(1);
	}
	miLogInfo("Agrego subdirectorio Bitácoras");
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

int buscarposicionDisponible(int archivoMetadata){
    //Busca un lugar disponible en el archivo Blocks.ims
}

int cantPosicionesLibresEn (int posicionDisponible){
    //Devuelve cuantas posiciones libres hay en el bloque
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

