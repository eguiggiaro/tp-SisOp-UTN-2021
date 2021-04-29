#ifndef DISCORDIADOR_H
#define DISCORDIADOR_H

#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
void atender_request_discordiador(uint32_t request_fd);
t_log* logger;
#endif