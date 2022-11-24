#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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
bool insert = true;

/* fifo section */
int reader_create_fifo(char* myfifo){
	printf("logger creating fifo \n");
	// creating the named file(FIFO), mkfifo(<pathname>,<permission>)
        fdl =mkfifo(myfifo,0666);
	printf("logger fifo created");
        return fdl;
}

// not used
void reader_open_and_write_fifo(char* myfifo, char* message){
        // open fifo for write only
        if(myfifo != NULL){
		printf("logger fifo is not null \n");
                // fifo create succeed
                fdl = open(myfifo,O_WRONLY);
	        /* clear O_NONBLOCK to avoid continuous writing during later scan */
        	/*fcntl(fdl,F_GETFL);
        	int flags = 0;
        	flags |= O_NONBLOCK;
        	fcntl(fdl,F_SETFL,flags);*/
		printf("logger fd table configured, %d \n", fdl);
                // write input on fifo and close it
                write(fdl, message, strlen(message)+1);
                close(fdl);
        }
        else{printf("logger create fifo failed. exit \n");
        exit(0);}
}

int reader_open_and_read_fifo(char* myfifo){
	// open log file
	FILE* log = open_log(logname, insert);
        // open fifo for read only
        fdl = open(myfifo, O_RDONLY);
	char message[MAX_BUFF];
	int nbytes;
	if(fdl > 0){
		printf("logger fd table configured, %d \n", fdl);
                //read from fifo succeed
                nbytes = read(fdl,message,MAX_BUFF);
                // print to stdout the read message
                printf("logger recieved: %s\n", message);
                close(fdl);
        }else{
        	perror("logger read fifo failed. exit \n");
        	exit(0);
	}
	log_event(log, message);
	int err = close_log(log);
	if(err != 0){printf("logger closing log file failed. err = %d \n", err); exit(0);}
	return nbytes;
}

FILE* open_log(char* filename, bool append){
        /* filename of the csv file
        bool: csv file exist, overwritten = false; exist: append = true; */
        FILE* fileptr = fopen(filename, ((append == true)? "a+": "w+"));
        return fileptr;
}

void log_event(FILE* log, char* message){
	time_t t = time(&t);
	char msg[MAX_BUFF];
	char* msgptr = msg;
	for(int i = 0; i < sizeof(message); i++){
		asprintf(&msgptr,"%d",toascii(message[i]));
	}
	fputs(msg,log);
	free(msgptr);
	printf("logger logging into gateway succeed\n");
}

int close_log(FILE* log){
	fclose(log);
	return 0;
}
int reader_get_fd(){return fdl;}



