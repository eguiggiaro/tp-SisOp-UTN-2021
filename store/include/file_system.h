#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "store.h"

uint32_t cantidadBloques;
uint32_t tamanioBloque; 

void verificar_fs(Configuracion*);
void crearSuperbloque(char*);
void crearBlocks(char*);
void crearArbolDirectorios(char*);
void leerSuperBloque(char*);

void inicializarBitmap(FILE*);

#endif