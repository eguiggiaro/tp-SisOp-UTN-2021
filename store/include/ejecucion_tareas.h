#ifndef EJECUCION_TAREAS_H
#define EJECUCION_TAREAS_H

//Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

//Commons libs
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

//Shared libs
#include "shared_utils.h"
#include "store.h"

void generarRecursos(tipoRecurso, int);
t_list* llenarBloque(int size, int blockCount, int ultimoBloque, char* cadenaCaracteres);
char * generarCadenaCaracteres(tipoRecurso recurso, int cantidad);
int bytesLibresUltimoBloque(int size, int blockCount);
void verificarMetadata(tipoRecurso recurso);
char * truncarCadenaDesdeCantidad(char* cadenaCaracteres, int cantidad);
char * truncarCadenaHastaCantidad(char* cadenaCaracteres, int cantidad);

#endif