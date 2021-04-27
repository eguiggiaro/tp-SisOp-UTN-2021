#include "store.h"

int main(){
    t_log* logger = log_create("./cfg/store.log", "STORE", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el store! %s", mi_funcion_compartida());
    log_destroy(logger);
}