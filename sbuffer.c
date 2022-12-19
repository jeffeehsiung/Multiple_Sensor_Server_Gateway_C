/**
 * \author Jeffee Hsiung
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "sbuffer.h"

/* Semaphore variables */
sem_t mutex;   // to esnure mutual exclusion when readcoutn is updated
sem_t wrt;  // common to both raeder and writer processes
int readercount = 0;
int writercount = 0;
FILE* csv;

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
*/
typedef struct sbuffer_node {
    struct sbuffer_node* next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
}sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t* head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t* tail;       /**< a pointer to the last node in the buffer */
    bool end_of_stream;
    bool finish_reading;
};

int sbuffer_init(sbuffer_t** buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    memset(*buffer,'\0',sizeof(sbuffer_t));

    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    csv = fopen("sensor_data_out.csv", "a");
    //printf("sbuffer init csv addr: %p\n",csv);
    if(ferror(csv)){
        perror("error opening csv\n"); exit(EXIT_FAILURE);
    }
    /* initialize the binary semaphore to 1 and set shared between threads */
    if (sem_init(&mutex, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }
    if (sem_init(&wrt, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t** buffer) {
    sbuffer_node_t* dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    fclose(csv);
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t* buffer, sensor_data_t* data) {
    sbuffer_node_t* dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;

    // lock to update readcount
    sem_wait(&mutex);
    readercount++;

    // head empty, reader leaves
    bool flag = sbuffer_get_end(buffer);
    if ((buffer->head == NULL) && (flag == false)){
        readercount--;
        if (readercount == 0) {
            sem_post(&wrt);
        }
        sem_post(&mutex);

        return SBUFFER_NO_DATA;
    }
    else if ((buffer->head == NULL) && (sbuffer_get_end(buffer) == true)){
        readercount--;
        sem_post(&mutex);
        return SBUFFER_END;
    }

    /* critical section */
    sem_wait(&wrt);
    data = &(buffer->head->data);
    fprintf(csv,"%hu,%lf,%ld\n", (data)->id, (data)->value, (data)->ts);

    // if last reader has read, remove the node
    if(sbuffer_get_finish(buffer) == true){
        dummy = buffer->head;
        // move head to next node for the other reader
        if (buffer->head == buffer->tail){ // buffer has only one node
            buffer->head = buffer->tail = NULL;
        }
        else{
            buffer->head = buffer->head->next; // regardless of if next node is null, next cycle will detect
        }
        free(dummy);
    }
    /* end of critical section */
    readercount--;
    sem_post(&wrt);
    sem_post(&mutex);

    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t* buffer, sensor_data_t* data) {
    // lock to exclude any other writer or reader
    sem_wait(&wrt);
    writercount++;

    sbuffer_node_t* dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy; // insert to tail
        buffer->tail = buffer->tail->next; // point tail to the inserted end
    }

    // unlock
    writercount--;
    sem_post(&wrt);
    return SBUFFER_SUCCESS;
}

void sbuffer_set_end(sbuffer_t* buffer, bool flag){
    buffer->end_of_stream = flag;
}

bool sbuffer_get_end(sbuffer_t* buffer){
    return buffer->end_of_stream;
}

void sbuffer_set_finish(sbuffer_t* buffer, bool flag){
    buffer->finish_reading = flag;
}

bool sbuffer_get_finish(sbuffer_t* buffer){
    return buffer->finish_reading;
}



