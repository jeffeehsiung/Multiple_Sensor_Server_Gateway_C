/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <aio.h>
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "sbuffer.h"



void print_help(void);

/* global varaibles */
int fd[2]; 
sem_t pipe_lock;
sbuffer_t* buffer;


int main(int argc, char *argv[]){

    pid_t pid;
    int server_port;
    // bool terminate = false;

    if (argc != 2) {
        print_help();
        exit(EXIT_SUCCESS);
    } else {
        server_port = atoi(argv[1]);
    }
	// create the pipe
	if (pipe(fd) == -1) {
		perror("pipe creation failed\n"); exit(EXIT_FAILURE);
	}

	// fork a child process
    pid = fork();
    if (pid < 0){
        perror("Fork failed.\n"); exit(EXIT_FAILURE);
    }

	// parent process: main process
	if (pid > 0){

        int totalthread = 0;
        pthread_t threads[MAX_RD + MAX_WRT];
        
        // initialize joinable threads
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

        if (sem_init(&pipe_lock, 0, 1) == -1){
            perror("sem_init failed\n"); exit(EXIT_FAILURE);
        }

        if (sbuffer_init(&buffer) != SBUFFER_SUCCESS){
            perror("sbuffer_init failed\n"); exit(EXIT_FAILURE);
        }

        close(fd[READ_END]);

        pthread_create(&threads[totalthread],&attr,connmgr_start,(void*) &server_port); totalthread++; 
        pthread_create(&threads[totalthread],&attr,datamgr_start,(void*) buffer); totalthread++;
        pthread_create(&threads[totalthread],&attr,sensor_db_start,(void*) buffer); totalthread++;

        
        while (totalthread >  0) {
            if(pthread_join(threads[totalthread-1],NULL) != 0){
                perror("main: failed to detach thread \n"); exit(EXIT_FAILURE);
            }else{
                totalthread--;
                printf(" main: threads in main left active: %d:\n",totalthread);
            }
        }


        printf("main: all threads terminated\n");

        if (sbuffer_free(&buffer) != SBUFFER_SUCCESS){
            perror("sbuffer_free failed\n"); exit(EXIT_FAILURE);
        }

        if (sem_destroy(&pipe_lock) == -1){
            perror("sem_destroy failed\n"); exit(EXIT_FAILURE);
        }

        close(fd[WRITE_END]);

        // destroy the attribute
        pthread_attr_destroy(&attr);

        wait(NULL);

        printf("main: logger process terminated\n");
        
        // exit(EXIT_SUCCESS);

        printf("main: main process terminating\n");

        return 0;

    }
	// child process: log process
    else
    {
        close(fd[WRITE_END]);

        int seq_num = 0;
        char read_msg[BUFF_SIZE];

        FILE *log = fopen("gateway.log", "a+");
        if (log == NULL)
        {
            perror("logger: opening file failed\n");
            exit(EXIT_FAILURE);
        }

        // read from the pipe
        while ((read(fd[READ_END], read_msg, sizeof(read_msg))) > 0)
        {
            // write an ASCII message to the log file in the format: <seq_num> <timestamp> <message>
            time_t t;
            time(&t);
            fprintf(log, "%d %s %s", seq_num, ctime(&t), read_msg);
            fflush(log);

            seq_num++;
        }
        printf("logger: read 0 bytes from pipe\n");

        close(fd[READ_END]);

        if (fclose(log) != 0)
        {
            perror("logger closing file falied\n");
            exit(EXIT_FAILURE);
        }

        printf("logger: process terminated\n");

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

// void set_termintate(bool term){
//     terminate = term;
// }

// bool get_terminate(void){
//     return terminate;
// }



