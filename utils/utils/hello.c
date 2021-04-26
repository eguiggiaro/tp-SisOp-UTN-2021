#include "hello.h"

void helloWorld() {
    puts("Hello world!!");

    t_log* logger;
    logger = log_create("prueba.log", "I-Mongo-Store", true, LOG_LEVEL_INFO);
    log_info(logger, "Prueba");
    log_destroy(logger);
}
