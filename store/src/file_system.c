#include "file_system.h"

void verificar_fs(char* punto_montaje){

	//leerSuperBloque(punto_montaje);

	char* pathSuperbloque = string_from_format("%s/%s", punto_montaje, "SuperBloque.ims"); //ej: /home/utnso/mnt/SuperBloque.ims

	if(fopen(pathSuperbloque, "r") == NULL){
		//No encontre el archivo de bloques. Lo creo.
		crearSuperBloque(punto_montaje);
		crearBlocks(punto_montaje);
		limpiarBitmap(punto_montaje);

	} else {

        miLogDebug("Ya existe el archivo Blocks.ims\n"); //Trace?
	}

	char* pathFiles = string_from_format("%s/%s", punto_montaje, "Files");
	
	DIR* dirFiles = opendir(pathFiles);
	if (dirFiles) {
    	//El directorio existe.
    	closedir(dirFiles);
	} else {
		//La estructura de directorios no existe. La creo.
		crearArbolDirectorios(pathFiles);
	}

	free(pathSuperbloque);    
	free(pathFiles); 
    
}

/*void leerSuperBloque(char* punto_montaje){

	char* pathSuperbloque = string_from_format("%s/%s", punto_montaje, "SuperBloque.ims");
	t_config* md = config_create(pathSuperbloque);

	if(md == NULL) {
		miLogError("No se pudo obtener el archivo metadata desde: %s", pathSuperbloque);
		exit(-1);
	}
	tamanioBloque = config_get_int_value(md, "BLOCK_SIZE");
	cantidadBloques = config_get_int_value(md, "BLOCKS");

	config_destroy(md);
	free(pathSuperbloque);
}*/

void crearSuperBloque(char* punto_montaje){
	
	char* pathSuperbloque = string_from_format("%s/%s", punto_montaje, "SuperBloque.ims");
	FILE* archivoSuperbloque = fopen(pathSuperbloque, "wb");

	//TODO: llenar el archivo con algo que no se..

	fclose(archivoSuperbloque);
	free(pathSuperbloque);
}


void crearBlocks(char* punto_montaje){
	
	char* pathBlocks = string_from_format("%s/%s", punto_montaje, "Blocks.ims");
	FILE* archivoBlocks = fopen(pathBlocks, "wb");

	//TODO: llenar el archivo con algo que no se..

	fclose(archivoBlocks);
	free(pathBlocks);
}

void crearArbolDirectorios(char* pathFiles){

	char* pathBitacoras = string_from_format("%s/%s", pathFiles, "Bitacoras");

	mkdir(pathFiles, 0777);
	miLogInfo("Agrego directorio Files");
	mkdir(pathBitacoras, 0777);
	miLogInfo("Agrego subdirectorio Bitácoras");

	free(pathBitacoras);
}

void limpiarBitmap(char* punto_montaje){
	
	char* pathSuperbloque = string_from_format("%s/%s", punto_montaje, "SuperBloque.ims");
	void* punteroABitmap = calloc(cantidadBloques/8, 1);

	FILE* superBloqueFile = fopen(pathSuperbloque, "w+");
	
	fwrite(punteroABitmap, 1, cantidadBloques/8, superBloqueFile);
	fclose(superBloqueFile);

	free(punteroABitmap);
	free(superBloqueFile);

	miLogInfo("Limpió el bitmap del archivo SuperBloque.");
}