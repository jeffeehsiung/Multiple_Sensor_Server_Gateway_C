/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <aio.h>
#include "config.h"
#include "sbuffer.h"
#include "logger.h"
#include "connmgr.h"

void print_help(void);

/* global varaibles */
int fd[2]; // two ends of a file description for read and write

/* threads variables */
pthread_t threads[MAX_RD + MAX_WRT];


int main(int argc, char *argv[]){
    /* buf */
	char write_msg[MAX_BUFF];
	char read_msg[MAX_BUFF];

	/* instantiate */
	pid_t pid;
    int server_port;

    if (argc != 2) {
        print_help();
        exit(EXIT_SUCCESS);
    } else {
        server_port = atoi(argv[1]);
    }
	/* create the pipe */
	if (pipe(fd) == -1) {
		perror("pipe creation failed\n"); exit(EXIT_FAILURE);
	}

	/* fork a child process */
    pid = fork();
    if (pid < 0){
        perror("Fork failed.\n"); exit(EXIT_FAILURE);
    }

	/* parent process: main process */
	if (pid > 0){

        /** instantiate variables */
        int totalthread = 0;

        /* close the read end of the pipe */
        close(fd[READ_END]);

        /* create connmgr thread */
        do {
            printf("creating %d -th thread\n", totalthread);
            if (pthread_create(&threads[totalthread],NULL,connmgr_start,server_port) != 0){
                perror("failed to create thread \n"); exit(EXIT_FAILURE);
            }
            totalthread++;
        } while (totalthread < MAX_WRT);

        /* wait for target threads to terminate */
        while (totalthread >  0) {
            pthread_join(threads[totalthread],NULL);
            totalthread--;
            printf("number of threads left active: %d\n", totalthread);
        }

        /* wait for child process to terminate */
        wait(NULL);

        /* exit parent process */
        exit(EXIT_SUCCESS);
    }
	/* child process: log process */
    else{
        /* close the writing end of the pipe */
        close(fd[WRITE_END]);
        /* open logfile and read until there is nothing then close it */
        bool append = true;
        FILE* log = open_log(append);
        /* read from the pipe into the buf*/
        while(read(fd[READ_END], read_msg, sizeof(read_msg)) > 0){
            //TODO should get the bytes that's gonna be read for read function
            log_event(log,read_msg);
        }
        close_log(log);

        /* close the child reading end of the pipe*/
        close(fd[READ_END]);

        /* exit child process */
        exit(EXIT_SUCCESS);
	}
    
    return 0;
}

/**
 * Helper method to print a message on how to use this application
 */
void print_help(void) {
    printf("Use this program with 2 command line options: \n");
    printf("\t%-15s : TCP server port number\n", "\'server port\'");
}
