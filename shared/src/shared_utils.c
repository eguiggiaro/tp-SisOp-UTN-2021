#include "shared_utils.h"

char* mi_funcion_compartida(){
    return "Hice uso de la shared!";
}

void* metodo() {
t_log* logger = log_create("./cfg/discordiador.log", "Discordiador", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el discordiador desde utils! %s", mi_funcion_compartida());
    log_destroy(logger);
}