#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "store.h"

char* puntoMontaje;
uint32_t cantidadBloques;
uint32_t tamanioBloque; 
t_bitarray* bitmap;

void verificar_fs(Configuracion*);
void crearSuperbloque(char*);
void crearBlocks(char*);
void crearArbolDirectorios(char*);
void leerSuperBloque(char*);

//void llenarBitmapTest();
void setearBitmapAUno();
void leerBitmap();

#endif