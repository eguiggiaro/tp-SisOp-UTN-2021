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


/*----------------------------------------------*/
bool verificarFS(void);
bool verificarEstructuraDirectorios(void);
void crearSuperbloque(void);
void crearBlocks(void);
void crearArbolDirectorios(void);
void leerSuperbloque(void);
int buscarYAsignarProximoBloqueLibre(void);
void liberarBloque(int);
int tamanioArchivo(char*);



void finalizarFS(void);
void leerSuperBloqueSinMapear(void);

#endif