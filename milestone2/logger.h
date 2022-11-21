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
//#include "config.h"
//#include "sensor_db.h"

/* fifo create */
char* reader_create_fifo(char* myfifo);

/* open fifo and write to fifo*/
void reader_open_and_write_fifo(char* myfifo, char* message);

/*open fifo and read from fifo*/
char* reader_open_and_read_fifo(char* myfifo, char* messgae);

/*create the log file*/
FILE* open_log(char* filename, bool append);

/* log event process */
FILE* log_event(char* myfifo, FILE* log, char* message);

/* close the log file and free the heap log memory */
int close_log(FILE* log);

int reader_get_fd();
#endif /* _LOGGER_H_ */
