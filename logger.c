#include <stdio.h>
#include <stdlib.h>
#include "logger.h"


#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null
#define MAX_BUFF 1024
#define NEWCSV 97
#define APPENDCSV 98
#define INSERTED 99
#define INSERTFAIL 100
#define CSVCLOSED 101

//char* myfifo; // used in create fifo
int fdl;
int id = 0;
char* logname = "gateway.log";

FILE* open_log(bool append){
        /* bool: csv file exist, overwritten = false; exist: append = true; */
        FILE* fileptr = fopen(logname, ((append == true)? "a+": "w+"));
        return fileptr;
}

/** strlen vs sizeof: 
 * https://www.sanfoundry.com/c-tutorials-size-array-using-sizeof-operator-strlen-function/
 * */
void log_event(FILE* log, char* message){
	fwrite(message,strlen(message)+1,1,log); //fwrite write in ascii format
	fprintf("logger logged: %s\n",message);
}

void close_log(FILE* log){
	fclose(log);
	perror("logger closing file falied\n"); exit(EXIT_FAILURE);
}



