#include "miram.h"

int main(){
    t_log* logger = log_create("./cfg/miram.log", "MiRAM", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy MiRAM! %s", mi_funcion_compartida());
    log_destroy(logger);
}