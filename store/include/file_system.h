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
int tamanioArchivo(char*);
int calcularCantBloques(int);
t_list* escribirBloquesNuevo(char*);
void escribirBloque(int, int, char*);
void escribirBloqueUsado(int, int , char*);
char* leerBloque(int , int);
char* leerBloques(t_list*, int);





void finalizarFS(void);
void leerSuperBloqueSinMapear(void);

#endif