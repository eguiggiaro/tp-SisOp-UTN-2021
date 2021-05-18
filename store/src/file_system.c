#include "file_system.h"


void verificar_fs(Configuracion* config){

	char* puntoMontaje = config->puntoMontaje;
	char* pathSuperbloque = string_from_format("%s/%s", puntoMontaje, "SuperBloque.ims"); //ej: /home/utnso/mnt/SuperBloque.ims
	char* pathBlocks = string_from_format("%s/%s", puntoMontaje, "Blocks.ims");
		
	cantidadBloques = config->blocksQtyDefault;
	tamanioBloque = config->blockSizeDefault;

	if(fopen(pathSuperbloque, "r") == NULL || fopen(pathBlocks, "r") == NULL){
		//No encontre el archivo de Superbloques. Creo el FS.
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
	uint32_t* buffer;

	fseek(archivoSuperbloque, 0, SEEK_SET);
	fread(buffer, sizeof(uint32_t), 1, archivoSuperbloque);
	tamanioBloque = *buffer;

	fseek(archivoSuperbloque, 0, SEEK_CUR);
	fread(buffer, sizeof(uint32_t), 1, archivoSuperbloque);
	cantidadBloques = *buffer;

	//TODO: Aca tengo que cargar el bitmap en un bitarray.

	fclose(archivoSuperbloque);
}