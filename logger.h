#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/*create the log file*/
FILE* open_log(bool append);

/* log event process */
void log_event(FILE* log, char* message);

/* close the log file and free the heap log memory */
void close_log(FILE* log);

#endif /* _LOGGER_H_ */
