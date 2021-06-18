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
sem_t sem_bitmap;
sem_t sem_bloques;


/*----------------------------------------------*/
bool verificarFS(void);
void crearSuperbloque(void);
void crearBlocks(void);
void crearDirectorio(char*);
void crearArbolDirectorios(void);
void leerSuperbloque(void);
void subirBlocksAMemoria();

int buscarYAsignarProximoBloqueLibre(void);
void liberarBloque(int);
int calcularCantBloques(int);
t_list* escribirBloquesNuevo(char*);
void escribirBloque(int, int, char*);
void escribirBloqueUsado(int, int , char*);
char* leerBloque(int , int);
char* leerBloques(t_list*, int);

MetadataBitacora* leerMetadataBitacora(int);
MetadataRecurso* leerMetadataRecurso(tipoRecurso);
char* obtenerDireccionDeMetadataRecurso (tipoRecurso);
char* obtenerDireccionDeMetadataBitacora (int);
int modificarMetadataRecurso(MetadataRecurso*, tipoRecurso);
int modificarMetadataBitacora(MetadataBitacora*, int);
char* generarMd5(t_list*);

void finalizarFS(void);

/**** SOPORTE ****/
t_list* listaFromArray(char**);
int crearMetadata(char*);
int tamanioArchivo(char*);


/**** DEṔRECADAS ****/
void leerSuperBloqueSinMapear(void);

#endif