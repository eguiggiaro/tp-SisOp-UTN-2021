#ifndef STORE_H_
#define STORE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <utils/logger.h>

typedef struct Configuracion{
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
} Configuracion;

Configuracion* configuracion;

int leer_config(void);

#endif /* STORE_H_ */
