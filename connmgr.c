/**
 * \author {Jeffee Hsiung}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include "config.h"
#include "lib/tcpsock.h"

#define PORT 1234
#define MAX_CONN 3  // state the max. number of connections the server will handle before exiting

// initialize global variables
tcpsock_t *server,*client;

// Semaphore variables since each thread wishes to write to a sensor_data_t
sem_t x;

/**
 * The thread will begin control in this runner function for one client
 * code reference from 
 * https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
 * and 
 * https://www.geeksforgeeks.org/readers-writers-problem-set-1-introduction-and-readers-preference-solution/
 */
void* client_handler (void* param) {
    tcpsock_t* client = (tcpsock_t*) param;
    sensor_data_t data;
    int bytes, result;

    // lock the semaphore of data access
    if (sem_wait(&x) == -1){
        perror("sem_wait failed\n"); exit(EXIT_FAILURE);
    }
    int sval = 0;
    sem_getvalue(&x,&sval);
    printf("data access locked, current semaphore num: %d\n", sval);

    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);

        // prints to stdout (console)
        if ((result == TCP_NO_ERROR) && bytes) {
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,(long int) data.ts);
        }
    } while (result == TCP_NO_ERROR);
    
    // unlock the semaphore of data access
    if (sem_post(&x) == -1){
        perror("sem_post failed\n"); exit(EXIT_FAILURE);
    }
    sem_getvalue(&x,&sval);
    printf("data access unlocked, unlocked semaphore num: %d\n", sval);

    if (result == TCP_CONNECTION_CLOSED){printf("Peer has closed connection\n");}
    else{printf("Error occured on connection to peer\n");}

    tcp_close(&client);
    
    pthread_exit(NULL);
}

/**
 * Implements a sequential test server (only one connection at the same time)
 */
int main(void) {

    int conn_counter = 0;

    pthread_t clientthreads[MAX_CONN];

    /* initialize the semaphore with resource value 1 and pshared being 0, 
     * meaning shared between threads */
    if (sem_init(&x, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }

    /* open tcp connection */
    printf("Test server started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR){exit(EXIT_FAILURE);}

    //int i = 0;

    /* wait for each client and create thread for each client */
    do {
        // accept one client connection. on success, new client socket created
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR){
            exit(EXIT_FAILURE);
        }
        printf("Incoming client connection\n");

        // create client thread with socket number & start the runner + increment the conn_counter
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        printf("creating %d -th thread\n", conn_counter);
        if (pthread_create(&clientthreads[conn_counter],&attr,client_handler,client) != 0){
            printf("failed to create thread \n");
        }
        conn_counter++;
    } while (conn_counter < MAX_CONN);
    
    /* wait for target threads to terminate */
    while (conn_counter >  0) {
        while (conn_counter != 0){
            // join all threads
            pthread_join(clientthreads[conn_counter],NULL);
            printf("current tid index & num connected: %d\n", conn_counter);
            conn_counter--;
        }
    }

    /* tcp close connection fail safe */
    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");

    return 0;
}




