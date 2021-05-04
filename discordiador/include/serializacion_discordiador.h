#ifndef SERIALIZACION_DISCORDIADOR_H
#define SERIALIZACION_DISCORDIADOR_H

//Shared libs
#include "discordiador.h"
void enviar_accion_seleccionada(op_code codigo_operacion, uint32_t socket);
void expulsar_tripulante(uint32_t socket);
void iniciar_tripulante(uint32_t socket);
void informar_tareas_patota(uint32_t socket);
void mover_tripulante(uint32_t socket);
void tarea_siguiente(uint32_t socket);
#endif