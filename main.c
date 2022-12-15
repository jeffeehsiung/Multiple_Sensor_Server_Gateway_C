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
#include <semaphore.h>
#include <errno.h>
#include <stdint.h>
#include "config.h"
#include "sbuffer.h"
#include "logger.h"

#define MAX_BUFF	1024
#define READ_END	0
#define WRITE_END	1
#define MAX_RD      2
#define MAX_WRT     1


/* threads variables */
pthread_t threads[MAX_RD + MAX_WRT];


int main(void){
    /** buf */
	char write_msg[MAX_BUFF];
	char read_msg[MAX_BUFF];

	/** instantiate */
	pid_t pid;
	int fd[2]; // two ends of a file description for read and write

	/** create the pipe */
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
        sbuffer_t* sbuffer;
        int totalthread = 0;
        
        /* initialize malloced sbuffer */
        if (sbuffer_init(&sbuffer) < 0){
            perror("sbuffer_init failed\n"); exit(EXIT_FAILURE);
        }

		/* close the unused end of the pipe */
		close(fd[READ_END]);
		/* write to the pipe */
		write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
		/* close the write end of the pipe */
		close(fd[WRITE_END]);

        /* create writer thread */
        do {
            printf("creating %d -th thread\n", totalthread);
            if (pthread_create(&threads[totalthread],&attr,writer,sbuffer) != 0){
                perror("failed to create thread \n"); exit(EXIT_FAILURE);
            }
            totalthread++;
        } while (totalthread < MAX_WRT);

        /* create reader thread */
        do {
            printf("creating %d -th thread\n", totalthread);
            if (pthread_create(&threads[totalthread],&attr,reader,sbuffer) != 0){
                perror("failed to create thread \n"); exit(EXIT_FAILURE);
            }
            totalthread++;
        } while (totalthread < (MAX_RD+ MAX_WRT));

        /* wait for target threads to terminate */
        while (totalthread >  0) {
            pthread_join(threads[totalthread],NULL);
            totalthread--;
            printf("number of threads left active: %d\n", totalthread);
        }

        /* free the malloc */
        if (sbuffer_free(&sbuffer) < 0){ // shared buffer freed
            perror("sbuffer_free failed\n"); exit(EXIT_FAILURE);
        }
        pthread_exit(NULL);


    }
	/* child process: log process */
    else{
        /* close the unused end of the pipe */
        close(fd[WRITE_END]);
        /** open logfile and read until there is nothing then close it*/
        bool append = true;
        FILE* log = open_log(append);
        /* read from the pipe into the buf*/
        while(read(fd[READ_END], read_msg, sizeof(read_msg)) > 0){
            //TODO should get the bytes that's gonna be read for read function
            log_event(log,read_msg);
            /* close the write end of the pipe */
            close(fd[READ_END]);
        }
        close_log(log);
	}
    
    return 0;
}
