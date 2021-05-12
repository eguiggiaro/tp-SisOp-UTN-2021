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


void atender_request_store(uint32_t);


#endif