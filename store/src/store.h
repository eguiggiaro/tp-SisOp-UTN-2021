/*
 * store.h
 *
 *  Created on: 24 abr. 2021
 *      Author: utnso
 */

#ifndef _STORE_H_
#define _STORE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <utils/hello.h>

typedef struct Configuracion{
	char* puntoMontaje;
	int puerto;
	int tiempoSincro;
} Configuracion;

Configuracion* configuracion;

t_log* iniciar_logger(void);
int leer_config(t_log*);

#endif /* _STORE_H_ */
