#include "file_system.h"


void verificar_fs(Configuracion* config){

	puntoMontaje = config->puntoMontaje;
	cantidadBloques = config->blocksQtyDefault;
	tamanioBloque = config->blockSizeDefault;

	char* pathSuperbloque = string_from_format("%s/%s", puntoMontaje, "SuperBloque.ims"); //ej: /home/utnso/mnt/SuperBloque.ims
	char* pathBlocks = string_from_format("%s/%s", puntoMontaje, "Blocks.ims");
		

	if(fopen(pathSuperbloque, "r") == NULL || fopen(pathBlocks, "r") == NULL){
		//No encontre el archivo de Superbloques o el Blocks. Creo el FS.
		//Primero veo si esta la estructura de directorios.
		DIR* dirFiles = opendir(puntoMontaje);
		if (dirFiles) {
    	//El directorio existe.
    		closedir(dirFiles);
		} else {
		//La estructura de directorios no existe. La creo.
			crearArbolDirectorios(puntoMontaje);
		}
		crearSuperbloque(pathSuperbloque);
		crearBlocks(pathBlocks);
	} else {
		
        miLogDebug("Ya existe el archivo SuperBloque.ims\n"); //Trace?
	}

	leerSuperBloque(pathSuperbloque);

	free(puntoMontaje);    
	free(pathSuperbloque); 
	free(pathBlocks);    
}

void crearArbolDirectorios(char* puntoMontaje){

	char* pathFiles = string_from_format("%s/%s", puntoMontaje, "/Files");
	char* pathBitacoras = string_from_format("%s/%s", puntoMontaje, "/Files/Bitacoras");

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

	free(pathFiles);
	free(pathBitacoras);
}

void crearSuperbloque(char* pathSuperbloque){
	
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

void crearBlocks(char* pathBlocks){
	
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
	free(pathBlocks);
}

void inicializarBitmap(FILE* archivoSuperbloque){
	//Reservo memoria para el bitmap y lo inicializo en cero. El bitmap es a nivel bits, por eso lo divido por 8.
	//Despues lo voy a meter en un t_bitarray
	void* bitmap = calloc(cantidadBloques/8, 1);

	fwrite(bitmap, 1, cantidadBloques/8, archivoSuperbloque);
	miLogInfo("Inicializó el bitmap del archivo SuperBloque.");
	
	free(bitmap);
}

void leerSuperBloque(char* superbloques){
	FILE* archivoSuperbloque = fopen(superbloques, "r");
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

void setearBitmapAUno(){
	//char* pathSuperbloque = string_from_format("%s/%s", puntoMontaje, "SuperBloque.ims");
	//FILE* archivoSuperbloque = fopen(pathSuperbloque, "rw");
	int archivoSuperbloque = open("/home/utnso/mnt/SuperBloque.ims", O_RDWR);
	//Cargo a memoria el Bitmap. Para eso hago un puntero al archivo de superbloques, desde la posición donde arranca
	//el bitmap (START+8).
	int tamanioSuperbloque = cantidadBloques/8 + sizeof(uint32_t) * 2;
	char* punteroSuperbloque = mmap(NULL, tamanioSuperbloque, PROT_READ | PROT_WRITE, MAP_SHARED, archivoSuperbloque, 0);
	char* punteroBitmap = punteroSuperbloque + sizeof(uint32_t) * 2;

	t_bitarray* bitmap2 = bitarray_create_with_mode(punteroBitmap, cantidadBloques/8, MSB_FIRST);

	for(int i=0; i<cantidadBloques; i++){
		if(bitarray_test_bit(bitmap2, i) == 0){
			bitarray_set_bit(bitmap2 ,i);			
		}
	}

	msync(bitmap2->bitarray,cantidadBloques/8 ,0);
	bitarray_destroy(bitmap2);

	if (munmap(punteroSuperbloque, tamanioSuperbloque) == -1){ //desmapeo la copia del archivo bloque
        exit(1);//Fallo el desmapeo de copia
    }
}

void leerBitmap(){
	//char* pathSuperbloque = string_from_format("%s/%s", puntoMontaje, "SuperBloque.ims");
	//FILE* archivoSuperbloque = fopen(pathSuperbloque, "rw");
	int archivoSuperbloque = open("/home/utnso/mnt/SuperBloque.ims", O_RDWR);
	//Cargo a memoria el Bitmap. Para eso hago un puntero al archivo de superbloques, desde la posición donde arranca
	//el bitmap (START+8).
	int tamanioSuperbloque = cantidadBloques/8 + sizeof(uint32_t) * 2;
	char* punteroSuperbloque = mmap(NULL, tamanioSuperbloque, PROT_READ | PROT_WRITE, MAP_SHARED, archivoSuperbloque, 0);
	char* punteroBitmap = punteroSuperbloque + sizeof(uint32_t) * 2;

	t_bitarray* bitmap2 = bitarray_create_with_mode(punteroBitmap, cantidadBloques/8, MSB_FIRST);

	bool bit;
	for(int i=0; i<cantidadBloques; i++){
		bit = bitarray_test_bit(bitmap2, i);	
	}
	bitarray_destroy(bitmap2);

	if (munmap(punteroSuperbloque, tamanioSuperbloque) == -1){ //desmapeo la copia del archivo bloque
        exit(1);//Fallo el desmapeo de copia
    }
}

void escribirStringEnBlocks(char* texto){
	//bla...
}