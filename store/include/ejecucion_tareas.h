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

typedef enum
{
	OXIGENO,
    COMIDA,
    BASURA,
} tipoRecurso;

#endif