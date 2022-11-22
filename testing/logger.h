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

typedef struct {
    	int sequence;	/* logging sequecne */
    	time_t t;       /** looging time */
	int code;	/* logging error code */
} log_t;

/* reader create fifo*/
int reader_create_fifo(char* myfifo);

/* read from fifo and parse to log */
int  reader_read_and_parse_fifo(FILE* rd_stream);

/*create the log file*/
FILE* reader_open_log(char* filename, bool append);

/* logger parse str to log */
void reader_parse_log(FILE* log, char* buf);

/* close the log file and free the heap log memory */
int close_log(FILE* log);

int reader_get_fd();
#endif /* _LOGGER_H_ */
