#ifndef REQUEST_ANALYZER_H
#define REQUEST_ANALYZER_H

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
#include "paquete.h"
#include "store.h"


void atender_request_store(Request *request);
tipoTarea find_enum_consola(char*);
int ejecutarTarea(char* tarea, int cantidadRecursos);


#endif