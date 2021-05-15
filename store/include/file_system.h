#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

//Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <dirent.h>

//Commons libs
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

//Shared libs
#include "shared_utils.h"


uint32_t cantidadBloques;
uint32_t tamanioBloque;


void verificar_fs(char*);
void leerSuperBloque(char*);

void crearArbolDirectorios(char*);
void crearSuperbloque(char*);
void crearBlocks(char*);
void limpiarBitmap(char*);

#endif