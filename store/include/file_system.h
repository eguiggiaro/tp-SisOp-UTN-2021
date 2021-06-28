#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "store.h"

#define SUPERBLOQUE_FILE "Superbloques.ims"
#define BLOCKS_FILE "Blocks.ims"

//Paths
char* puntoMontaje;
char* pathSuperbloque;
char* pathBlocks;
char* pathFiles;
char* pathBitacoras;

//Superbloque
uint32_t tamanioBloque; 
uint32_t cantidadBloques;
t_bitarray* bitmap;

//Blocks
char* punteroBlocks;

//Semáforos
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_bloques;
pthread_mutex_t mutex_escribirBloques;


/**** MANEJO FILE SYSTEM ****/
bool verificarFS();
void crearSuperbloque();
void crearBlocks();
void crearDirectorio(char*);
void crearArbolDirectorios();
void leerSuperbloque();
void subirBlocksAMemoria();
void finalizarFS();

/**** MANEJO BLOQUES ****/
int buscarYAsignarProximoBloqueLibre();
void liberarBloque(int);
int calcularCantBloques(int);
t_list* escribirBloquesNuevo(char*);
void escribirBloque(int, int, char*);
void escribirBloqueUsado(int, int , char*);
char* leerBloque(int , int);
char* leerBloques(t_list*, int);
int verificarQueTengoLosBloquesNecesarios(int);

/**** MANEJO METADATA ****/
MetadataBitacora* leerMetadataBitacora(char*);
MetadataRecurso* leerMetadataRecurso(tipoRecurso);
char* obtenerDireccionDeMetadataRecurso(tipoRecurso);
char* obtenerDireccionDeMetadataBitacora(char*);
int modificarMetadataRecurso(MetadataRecurso*, tipoRecurso);
int modificarMetadataBitacora(MetadataBitacora*, char*);
int crearMetadataRecurso(tipoRecurso);
int crearMetadataBitacora(char*);

/**** SOPORTE ****/
t_list* listaFromArray(char**);
int verificarExistenciaFile(char*);
int tamanioArchivo(char*);
char* stringFromList(t_list*);
char* generarMd5(t_list*, int); //TODO
void borrarTodosLosArchivos(char*);


/**** DEṔRECADAS ****/
void leerSuperBloqueSinMapear(void);

#endif