/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "config.h"
#include "sbuffer.h"

#define MAX_RD 2
#define MAX_WRT 1

/* Semaphore variables */
sem_t mutex;   // to esnure mutual exclusion when readcoutn is updated
sem_t wrt;  // common to both raeder and writer processes
pthread_t threads[MAX_RD + MAX_WRT];
int readercount = 0;
int writercount = 0;

// Reader Function
void* reader(void* param)
{
    (FILE*) file = (FILE*) param[0];
    (sbuffer_t*) buffer = (sbuffer_t*) param[1];

    /* for sbuffer */
    // lock to update readcount
    sem_wait(&mutex);
    readercount++;
 
    if (readercount != 0){
        sem_wait(&wrt);     // no writer can enter
        sem_post(&mutex);   // other reader can come in
    }
    printf("\n%d reader is inside", readercount);
    sleep(1);
    
    /* current reader performs reading here */
    // read sbuffer, to be implemented

    // lock to update readcount
    sem_wait(&mutex);
    readercount--;
 
    if (readercount == 0) { // no reader is left in the critical section
        sem_post(&wrt);
    }
 
    // unlock reader semaphore
    sem_post(&mutex);

    /* for sensor_data_out */
    //TODO

    pthread_exit(NULL);
}

// Writer Function
void* writer(void* param)
{
    printf("\nWriter is trying to enter");
    (sbuffer_t*) buffer = (sbuffer_t*) param[1];

    // lock to exclude any other writer or reader
    sem_wait(&wrt);
    writercount++;
 
    printf("\nWriter has entered");
 
    // unlock
    sem_post(&wrt);
    writercount--;
 
    printf("\nWriter is leaving");
    pthread_exit(NULL);
}

// Driver code
int main(int argc, char *argv[]){
    
    /* initialize variables */
    FILE* sensor_data_out;
    sbuffer_t* sbuffer;
    sensor_data_t data;
    void* ptrs = {(void*) sensor_data_out, (void*) sbuffer};

    int totalthread = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    sensor_data_out = fopen("sensor_data_out.txt", "a+");
    if(ferror(sensor_data_out)){
        perror("error opening file\n"); exit(EXIT_FAILURE);
    }
    /* initialize the binary semaphore to 1 and set shared between threads */
    if (sem_init(&mutex, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }
    if (sem_init(&wrt, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }
    /* initialize malloced sbuffer */
    if (sbuffer_init(&sbuffer) < 0){
        perror("sbuffer_init failed\n"); exit(EXIT_FAILURE);
    };

    /* create writer thread */
    do {
        printf("creating %d -th thread\n", totalthread);
        if (pthread_create(&threads[totalthread],&attr,writer,ptrs) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++;
    } while (writercount < MAX_WRT);

    /* create reader thread */
    do {
        printf("creating %d -th thread\n", totalthread);
        if (pthread_create(&threads[totalthread],&attr,reader,ptrs) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++;
    } while (readercount < MAX_RD);

    /* wait for target threads to terminate */
    while (totalthread >  0) {
        pthread_join(threads[totalthread],NULL);
        totalthread--;
    }


    /* free the malloc */
    if (sbuffer_free(&sbuffer) < 0){ // shared buffer freed
        perror("sbuffer_free failed\n"); exit(EXIT_FAILURE);
    };

    /* file close fail safe */
    fclose(sensor_data_out);

    return 0;
}