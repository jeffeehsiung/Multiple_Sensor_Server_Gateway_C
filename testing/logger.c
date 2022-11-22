#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "logger.h"

#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null
#define MAX_BUFF 2000
#define NEWCSV 97
#define APPENDCSV 98
#define INSERTED 99
#define INSERTFAIL 100
#define CSVCLOSED 101


/*
 * error code explain:
 * exit_failure -1 & exit_success 0
 */

int fdl;
/* reader create fifo */
int reader_create_fifo(char* myfifo){
	fdl = mkfifo(myfifo, 0666);
	if(fdl < 0){
		perror("reader create fifo failed: \t");
		exit(EXIT_FAILURE);
	}
	return fdl;
}
/* read and parse into buffer string*/
int reader_read_and_parse_fifo(FILE* rd_stream){
	char* filename = "gateway.log";
	bool append = true;
	FILE* log = reader_open_log(filename, append);
	/* assign buffer with size seeked on heap*/
	/* read iteration: number of bytes read is returned (zero indicates end of file) */
	char buf[MAX_BUFF];
	char* msg;
	char* buf2;
	//log_t* logger = malloc(sizeof(log_t));
	int strlength = 0;
	buf2 = fgets(buf,MAX_BUFF,rd_stream);
	while(strlen(buf)>0){
		/* generating msg */
		switch(buf2[0]){
			case NEWCSV:
				msg = "log-event: csv overwritten";
				asprintf(&buf2, "%s\n", strcat(buf,msg));
				break;
			case APPENDCSV:
                                msg = "log-event: append to csv";
                                asprintf(&buf2,"%s\n", strcat(buf,msg));
				break;
			case INSERTED:
                                msg = "log-event: sensor data insertion succeed";
                                asprintf(&buf2,"%s\n", strcat(buf,msg));
				break;
			case INSERTFAIL:
				msg = "log-event: sensor data insertion failed";
                                asprintf(&buf2,"%s\n", strcat(buf,msg));
				break;
			case CSVCLOSED:
                                msg = "log-event: csv closed";
                                asprintf(&buf2,"%s\n", strcat(buf,msg));
				break;
		}
		strlength = strlen(buf);
		/* logger parse str to log */
		reader_parse_log(log,buf);
		//free(logger);
		close_log(log);
	}
	/* alternative read by fgets, read from file and put to str, param needs FILE* */
	free(buf2);
	fclose(rd_stream);
        return strlength;
}

/* logger open log with fopen */
FILE* reader_open_log(char* filename, bool append){
        /* filename of the csv file
        bool: csv file exist, overwritten = false; exist: append = true; */
        FILE* fileptr = fopen(filename, ((append == true)? "a+": "w+"));
	if(fileptr == NULL){
		printf("logger log file open fail \n");
		exit(EXIT_FAILURE);
	}
        return fileptr;
}

/* logger parse str to log */
void reader_parse_log(FILE* log, char* buf){
	/* convert char to ascii */
	int ascii[sizeof(buf)];
	//char msg[sizeof(buf)];
	char* msg;
	printf("logger bufffer strlength to be converted: %ld \n", strlen(buf));
	for(int i = 0; i < sizeof(buf); i++){
		ascii[i] = buf[i];
		asprintf(&msg+i,"%ls",ascii);
		printf("logger ascii converted into %d \n", msg[i]);
	}
	/* puts to log */
	printf("logger logging into gateway \n");
	fputs(msg,log);
	printf("logger logging into gateway succeed\n");
}

int close_log(FILE* log){
	fclose(log);
	return 1; // 1 means true
}

int reader_get_fd(){ return
 fdl;}
