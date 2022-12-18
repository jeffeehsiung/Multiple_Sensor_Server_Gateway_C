/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include "connmgr.h"
#include "config.h"

#define MAX_CONN 4  // state the max. number of connections the server will handle before exiting

// initialize global variables
tcpsock_t *server,*client;
extern int fd[2];
// Semaphore variables since each thread wishes to write to a pipe
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
    int conncounter = 0;
    result = TCP_NO_ERROR;


    while (result == TCP_NO_ERROR){
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        
        conncounter++;
        if ((result == TCP_NO_ERROR) && bytes) {
            // write to pipe
            if(conncounter == 1){
                // lock the semaphore of data access
                if (sem_wait(&x) == -1){
                    perror("sem_wait failed\n"); exit(EXIT_FAILURE);
                }

                char buf[BUFF_SIZE];
                sprintf(buf,"Sensor node %d has opened a new connection\n",data.id);
                write(fd[WRITE_END],buf,sizeof(buf));

                // unlock the semaphore of data access
                if (sem_post(&x) == -1){
                    perror("sem_post failed\n"); exit(EXIT_FAILURE);
                }

            }
        }
    }
    
    if (result == TCP_CONNECTION_CLOSED){
        // write to pipe
        char buf[BUFF_SIZE];
        sprintf(buf,"Sensor node %d has closed the connection\n",data.id);
        write(fd[WRITE_END],buf,sizeof(buf));
    }
    else{printf("Error occured on connection to peer\n");}

    // close the client socket
    tcp_close(&client);
    // join connmgr thread
    pthread_exit(NULL);
}

/**
 * Implements a sequential test server (only one connection at the same time)
 */
void* connmgr_start(void* server_port) {
    
    /* typcast the void* to int* */
    int* port = (int*) server_port;
    
    /* initialize variables */
    int conn_counter = 0;

    /* initialize the semaphore shared between threads */
    if (sem_init(&x, 0, 1) == -1){
        perror("sem_init failed\n"); exit(EXIT_FAILURE);
    }

    /* initialize thread array */
    pthread_t clientthreads[MAX_CONN];

    /* open tcp connection */
    printf("Test server started\n");
    if (tcp_passive_open(&server, *port) != TCP_NO_ERROR){exit(EXIT_FAILURE);}


    /* wait for each client and create thread for each client */
    do {
        // accept one client connection. on success, new client socket created
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR){
            exit(EXIT_FAILURE);
        }
        
        // create client thread with socket number & start the runner + increment the conn_counter
        if (pthread_create(&clientthreads[conn_counter],NULL,client_handler,client) != 0){
            perror("failed to create thread \n"); exit(EXIT_FAILURE);
        }
        conn_counter++;
        // print the counter
        printf("connmgr conn_counter = %d \n",conn_counter);

    } while (conn_counter < MAX_CONN);
    
    /* wait for target threads to terminate */
    while (conn_counter >  0) {
        pthread_join(clientthreads[conn_counter-1],NULL);
        printf("connmgr joined thread number = %d \n",conn_counter);
        conn_counter--;
    }

    /* tcp close connection fail safe */
    if (tcp_close(&server) != TCP_NO_ERROR){exit(EXIT_FAILURE);}
    printf("Test server is shutting down\n");

    // join main thread
    pthread_exit(NULL);
    
}




