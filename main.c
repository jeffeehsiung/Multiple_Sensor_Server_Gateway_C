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

/* pipe varaibles */
int fd[2]; 

/* threads variables */
pthread_t threads[MAX_RD + MAX_WRT];

int main(int argc, char *argv[]){
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
        void* arg = (void*) &server_port;

        /* close the read end of the pipe */
        close(fd[READ_END]);

        /* create a thread that is joinable */
        if (pthread_create(&threads[totalthread],NULL,connmgr_start,arg) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++; 
        
        /* wait for target threads to terminate */
        while (totalthread >  0) {
            if(pthread_join(threads[totalthread-1],NULL) != 0){
                perror("main: failed to detach thread \n"); exit(EXIT_FAILURE);
            }else{
                totalthread--;
                printf(" main: threads in main left active: %d:",totalthread);
            }
        }

        /* wait for child process to terminate */
        wait(NULL);

	    /* close write end of the pipe */
	    close(fd[WRITE_END]);
        /* exit parent process */
        exit(EXIT_SUCCESS);

    }
	/* child process: log process */
    else
    {
        /* close the write end of the pipe */
        close(fd[WRITE_END]);

        // keep reading  until pipe is empty and log the message is the gateway.log file
        while (1)
        {

            FILE *log = fopen("gateway.log", "a");
            if (log == NULL)
            {
                perror("logger opening file failed\n");
                exit(EXIT_FAILURE);
            }

            // read from the pipe
            char read_msg[BUFF_SIZE];
            int bytes_read = read(fd[READ_END], read_msg, sizeof(read_msg));
            if (bytes_read == -1)
            {
                perror("logger reading from pipe failed\n");
                exit(EXIT_FAILURE);
            }
            if (bytes_read == 0)
            {
                printf("logger read 0 bytes from pipe\n");
                break;
            }

            fprintf(log, "%s", read_msg);
            fflush(log);

            if (fclose(log) != 0)
            {
                perror("logger closing file falied\n");
                exit(EXIT_FAILURE);
            }

        } 
        /* close the child reading end of the pipe*/
        close(fd[READ_END]);

        printf("logger process terminated\n");
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



