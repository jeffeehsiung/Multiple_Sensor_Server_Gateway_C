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
#define MAX_BUFF 2000
#define NEWCSV 97
#define APPENDCSV 98
#define INSERTED 99
#define INSERTFAIL 100
#define CSVCLOSED 101

//char* myfifo; // used in create fifo
int fdl;
int id = 0;

/* fifo section */
char* reader_create_fifo(char* myfifo){
	printf("logger creating fifo \n");
	// creating the named file(FIFO), mkfifo(<pathname>,<permission>)
        mkfifo(myfifo,0666);
        return myfifo;
}

// not used
void reader_open_and_write_fifo(char* myfifo, char* message){
        // open fifo for write only
        if(myfifo != NULL){
		printf("logger fifo is not null \n");
                // fifo create succeed
                fdl = open(myfifo,O_WRONLY);
		printf("logger fd table configured, %d \n", fdl);
                // write input on fifo and close it
                write(fdl, message, strlen(message)+1);
                close(fdl);
        }
        else{printf("logger create fifo failed. exit \n");
        exit(0);}
}

char* reader_open_and_read_fifo(char* myfifo, char* message){
        // open fifo for read only
        fdl = open(myfifo, O_RDONLY);
	//char* temp = malloc(sizeof(int)*MAX_BUFF);
        if(fdl > 0){
		printf("logger fd table configured, %d \n", fdl);
                //read from fifo succeed
                read(fdl,message,sizeof(message));
                // print to stdout the read message
                printf("logger recieved: %s\n", message);
                close(fdl);
		return message;
        }else{
        	printf("logger read fifo failed. exit \n");
        	exit(0);
	}
}

FILE* open_log(char* filename, bool append){
        /* filename of the csv file
        bool: csv file exist, overwritten = false; exist: append = true; */
        FILE* fileptr = fopen(filename, ((append == true)? "a+": "w+"));
        return fileptr;
}

FILE* log_event(char* myfifo, FILE* log, char* message){
	time_t t = time(&t);
	char* msg[MAX_BUFF];
	int ascii[strlen(message)+1];
	for(int i = 0; i < strlen(message)+1; i++){
		ascii[i] = message[i];
		printf("logger converting into %d \n", message[i]);
		asprintf(&(msg[i]),"%ls",ascii);
	}
	printf("logger converted msg: %s \n", *msg);
	printf("logger logging into gateway \n");
	fprintf(log, "%s \n", *msg);
	printf("logger logging into gateway succeed\n");
	return log;
}

int close_log(FILE* log){
	fclose(log);
	free(log);
	return 1; // 1 means true
}
int reader_get_fd(){return fdl;}


