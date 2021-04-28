#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>

//Campos en archivo de configuracion
#define LOG_LEVEL "LOG_LEVEL"
#define LOG_FILE "LOG_FILE"
#define LOG_PROGRAM_NAME "LOG_PROGRAM_NAME"
#define LOG_PRINT_CONSOLE "LOG_PRINT_CONSOLE"

void miLogInitMutex(char*, char*, bool, t_log_level);
void miLogInfo(const char*, ...);
void miLogDebug(const char*, ...);
void miLogTrace(const char*, ...);
void miLogWarning(const char*, ...);
void miLogError(const char*, ...);
void miLogDestroy();


#endif /* LOGGER_H_ */