#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

typedef struct{
	uint32_t PID;
	uint32_t Tareas;
} PCB;

typedef struct{
	uint32_t TID;
	char estado;
	uint32_t pos_X;
	uint32_t pos_y;
	uint32_t proxima_instruccion;
	uint32_t PCB;	
} TCB;

typedef struct Tarea{
	char* nombre_tarea;
	int parametro;
	int pos_x;
	int pos_y;
	int tiempo;
} Tarea;


typedef struct Punto{
	int pos_x;
	int pos_y;
} Punto;

void* MEMORIA;

int contador_patotas;
int contador_tripulantes;

#endif