/**
 * \author Jeffee Hsiung
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "sbuffer.h"

/* instantiate variables */
FILE* csv;

// /**
//  * basic node for the buffer, these nodes are linked together to create the buffer
// */
// typedef struct sbuffer_node {
//     sensor_data_t data;
//     struct sbuffer_node* next;
//     bool read_by_a;             
//     bool read_by_b;
// } sbuffer_node_t;


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
    if(pthread_mutex_init(&((*buffer)->mutex), NULL) != 0){
        perror("mutex init failed\n"); exit(EXIT_FAILURE);
    }
    if(pthread_cond_init(&((*buffer)->cond), NULL) != 0){
        perror("cond init failed\n"); exit(EXIT_FAILURE);
    }
    // pthread_mutex_destroy & pthread_cond_destroy to free the resources
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t** buffer) {
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        sbuffer_node_t* dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    // free the mutex and cond
    pthread_mutex_destroy(&((*buffer)->mutex));
    pthread_cond_destroy(&((*buffer)->cond));
    
    free(*buffer);
    *buffer = NULL;
    fclose(csv);
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t* buffer, sensor_data_t* data, int consumer_id) {

    if (buffer == NULL) return SBUFFER_FAILURE;

    // lock to secure the buffer->head and readercount
    pthread_mutex_lock(&buffer->mutex);

    // critical section
    // buufer empty, blocking wait for the producer to insert data
    while (buffer->head == NULL && buffer->end_of_stream == false){
        // pause the consumer thread and wait for the producer to insert data
        pthread_cond_wait(&buffer->cond, &buffer->mutex);
        // sleep for 5 seconds
        sleep(5);

        printf("consumer %d is waiting for data\n", consumer_id);
    }

    // buffer is empty and end of stream
    if (buffer->end_of_stream){
        pthread_mutex_unlock(&buffer->mutex);
        return SBUFFER_END;
    }
    
    // buffer is not empty
    if ((consumer_id == CONSUMER_A && buffer->head->read_by_a) || 
        (consumer_id == CONSUMER_B && buffer->head->read_by_b)){
        pthread_mutex_unlock(&buffer->mutex);
        return SBUFFER_NO_DATA;
    }
    
    data = &(buffer->head->data);
    fprintf(csv,"%hu,%lf,%ld\n", (data)->id, (data)->value, (data)->ts);
    printf("sbuffer_remove: printed data to csv\n");

    if (consumer_id == 0){
        buffer->head->read_by_a = true;
    } else if (consumer_id == 1){
        buffer->head->read_by_b = true;
    }

    // check if both consumer A and consumer B have read the data
    if (buffer->head->read_by_a && buffer->head->read_by_b){
        // remove the head node from the buffer, set the node to null, and free the memory
        sbuffer_node_t* dummy = buffer->head;
        buffer->head = buffer->head->next;
        free(dummy);
        printf(" sbuffer_remove: removed data from buffer\n");
    } 
    // end of critical section

    // release mutex lock
    pthread_mutex_unlock(&buffer->mutex);

    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t* buffer, sensor_data_t* data) {
    // lock to secure the buffer->tail
    //pthread_mutex_lock(&(buffer->mutex));
    
    if (buffer == NULL) return SBUFFER_FAILURE;
    
    sbuffer_node_t* dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    dummy->read_by_a = false;
    dummy->read_by_b = true; // set read_by_b to true for debugging

    //printf("inserted data: %hu,%lf,%ld\n", dummy->data.id, dummy->data.value, dummy->data.ts);
    
    if (buffer->tail == NULL) // buffer empty. buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy; // insert to tail
        buffer->tail = buffer->tail->next; // point tail to the inserted end
    }
    // signal the consumer threads that new data is available
    pthread_cond_broadcast(&(buffer->cond));

    // unlock the mutex
    //pthread_mutex_unlock(&(buffer->mutex));
    return SBUFFER_SUCCESS;
}

// function to set the end_of_stream flag
void sbuffer_set_end(sbuffer_t* buffer, bool end){
    buffer->end_of_stream = end;
}




