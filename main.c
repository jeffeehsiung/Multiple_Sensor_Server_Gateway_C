/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <inttypes.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <stdint.h>
#include "config.h"
#include "sbuffer.h"

#define MAX_RD 2
#define MAX_WRT 1

/* threads variables */
pthread_t threads[MAX_RD + MAX_WRT];


// Reader Function
void* reader(void* param){
    //printf("reader is trying to enter\n");
    sbuffer_t* buffer;
    buffer = (sbuffer_t*) (param);
    //printf("reader sbuffer addr: %p\n", buffer);
    sensor_data_t data;
    int count = 0;

    /* current reader performs reading here */
    int code = 5;
    while ((code != SBUFFER_END) && (code != SBUFFER_FAILURE)){
        //printf("reading sbuffer\n");
        code = sbuffer_remove(buffer,data);
        //printf("sbuffer_remove returned code: %d\n", code);
        if (code == SBUFFER_SUCCESS){
            count++;
        }
    }
    if (code == SBUFFER_END){
        printf("reading reached an end & rows appended: %d\n", count);
    }
    else if (code == SBUFFER_FAILURE){
        perror("buffer is NULL\n");
    }
    pthread_exit(NULL);
}

// Writer Function
void* writer(void* param){
    //printf("Writer is trying to enter\n");
    FILE* sensor_data = fopen("sensor_data", "rb");
    if(ferror(sensor_data)){
        perror("error opening sensor_data\n"); exit(EXIT_FAILURE);
    }
    sbuffer_t* buffer;
    buffer = (sbuffer_t*) (param);
    //printf("writer sbuffer addr: %p\n", buffer);
    sensor_data_t data;
    int count = 0;
    // mark end_of_stream of sbuffer as false
    sbuffer_setflag(buffer,false);
    // while loop: fread return number of item read
    while(fread(&data, sizeof(data), 1, sensor_data) > 0){
        if(sbuffer_insert(buffer,&data) < 0){
            perror("sbuffer_insert failed\n"); exit(EXIT_FAILURE);
        }
        count++;
    }
    // mark end_of_stream of sbuffer as true
    sbuffer_setflag(buffer,true);

    fclose(sensor_data);
    //printf("Writer has left & binary file closed & items appended: %d\n",count);
    pthread_exit(NULL);
}

// Driver code
int main(void){
    
    sbuffer_t* sbuffer;
 /*    void* (*readerptr)(void* sbuffer);
    readerptr = reader;
    void* (*writerptr)(void* sbuffer);
    writerptr = writer; */

    int totalthread = 0;

    /* initialize malloced sbuffer */
    if (sbuffer_init(&sbuffer) < 0){
        perror("sbuffer_init failed\n"); exit(EXIT_FAILURE);
    };
    //printf("main sbuffer addr: %p\n", sbuffer);

    /* create writer thread */
    do {
        //printf("creating %d -th thread\n", totalthread);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if (pthread_create(&threads[totalthread],&attr,writer,sbuffer) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++;
    } while (totalthread < MAX_WRT);

    /* create reader thread */
    do {
        //printf("creating %d -th thread\n", totalthread);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if (pthread_create(&threads[totalthread],&attr,reader,sbuffer) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        totalthread++;
    } while (totalthread < (MAX_RD+ MAX_WRT));

    /* wait for target threads to terminate */
    while (totalthread >  0) {
        pthread_join(threads[totalthread],NULL);
        totalthread--;
        printf("number of threads left active: %d", totalthread);
    }

    /* free the malloc */
    if (sbuffer_free(&sbuffer) < 0){ // shared buffer freed
        perror("sbuffer_free failed\n"); exit(EXIT_FAILURE);
    }

    return 0;
}