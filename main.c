/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <aio.h>
#include "config.h"
#include "sbuffer.h"
#include "connmgr.h"



void print_help(void);

/* global varaibles */
int fd[2]; // two ends of a file description for read and write

/* threads variables */
pthread_t threads[MAX_RD + MAX_WRT];


int main(int argc, char *argv[]){

	/* instantiate */
	pid_t pid;
    int server_port;
    // set pthread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

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
        void* arg = (void*) &server_port;

        /* close the read end of the pipe */
        close(fd[READ_END]);

        // create a thread that will start connmgr_start()
        if (pthread_create(&threads[totalthread],&attr,connmgr_start,arg) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++;

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
        char* logname = "gateway.log";
        bool append = true;
        FILE* log = fopen(logname, ((append == true)? "a+": "w+"));
        if (log == NULL){
            perror("logger opening file falied\n"); exit(EXIT_FAILURE);
        }

        /** strlen vs sizeof: 
         * https://www.sanfoundry.com/c-tutorials-size-array-using-sizeof-operator-strlen-function/
         * */

        /* read from the pipe into the buf*/
        char read_msg[100];
        while(read(fd[READ_END], read_msg, sizeof(read_msg)) > 0){
            fwrite(read_msg,strlen(read_msg)+1,1,log); //fwrite write in ascii format
        }

        if (fclose(log) != 0){
            perror("logger closing file falied\n"); exit(EXIT_FAILURE);
        }

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



