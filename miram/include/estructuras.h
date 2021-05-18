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

typedef struct {
	char* operacion;
	int parametro;
	int pos_x;
	int pos_y;
	int tiempo;
} tarea;



void* MEMORIA;

#endif