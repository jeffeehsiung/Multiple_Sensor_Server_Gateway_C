/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include "connmgr.h"
#include "config.h"
#include "sbuffer.h"

#define MAX_CONN 4  // state the max. number of connections the server will handle before exiting

// initialize global variables
tcpsock_t *server,*client;
extern int fd[2];
extern sem_t pipe_lock;
extern sbuffer_t* buffer;


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
    int counter = 0;
    result = TCP_NO_ERROR;

    // read data from the client and write to the pipe. 
    // If the sensor node has not sent new data within the defined timeout, the server closes the client socket and exits the thread
    while (result == TCP_NO_ERROR){
        // get the timestamp of the server start waiting for data
        time_t start = time(NULL);

        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        
        if ((result == TCP_NO_ERROR) && bytes) {
            // get the timestamp of the server end waiting for data
            time_t end = time(NULL);

            // insert the data into the buffer
            if(sbuffer_insert(buffer,&data) == SBUFFER_SUCCESS){
                counter++;
            }
            // write to pipe
            if(counter == 1){
                // lock the semaphore of data access
                if (sem_wait(&pipe_lock) == -1){
                    perror("sem_wait failed\n"); exit(EXIT_FAILURE);
                }
                char buf[BUFF_SIZE];
                sprintf(buf,"Sensor node %d has opened a new connection\n",data.id);
                write(fd[WRITE_END],buf,sizeof(buf));
                // unlock the semaphore of data access
                if (sem_post(&pipe_lock) == -1){
                    perror("sem_post failed\n"); exit(EXIT_FAILURE);
                }
            }
            // if the sensor node has not sent new data within the defined timeout, the server closes the client socket and breaks the loop
            if (end - start > TIMEOUT){
                printf("The sensor node %d has not sent new data within the defined timeout.\n", data.id);
                tcp_close(&client);
                result = TCP_CONNECTION_CLOSED;
                break;
            }
        }
    }

    if (result == TCP_CONNECTION_CLOSED){
        // lock the semaphore of data access
        if (sem_wait(&pipe_lock) == -1){
            perror("sem_wait failed\n"); exit(EXIT_FAILURE);
        }
        // write to pipe
        char buf[BUFF_SIZE];
        sprintf(buf,"Sensor node %d has closed the connection\n",data.id);
        write(fd[WRITE_END],buf,sizeof(buf));
        // unlock the semaphore of data access
        if (sem_post(&pipe_lock) == -1){
            perror("sem_post failed\n"); exit(EXIT_FAILURE);
        }
    }
    else{perror("Error occured on connection to peer\n");}

    printf("connmgr closed thread number = %d, total inserted: %d \n",data.id, counter);
    // close the client socket
    tcp_close(&client);

    // exit the thread
    pthread_exit(NULL);
}

/**
 * Implements a sequential test server (only one connection at the same time)
 */
void* connmgr_start(void* server_port) {
    
    // typcast the void* to int*
    int* port = (int*) server_port;
    
    // initialize variables
    int conn_counter = 0;
    
    // initialize thread array
    pthread_t clientthreads[MAX_CONN];

    // set the att of the thread to run into the background and release the resources when it terminates
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    // open tcp connection
    printf("Test server started\n");
    if (tcp_passive_open(&server, *port) != TCP_NO_ERROR){exit(EXIT_FAILURE);}

    // mark end_of_stream of sbuffer as false
    sbuffer_set_end(buffer,false);

    // wait for client to connect and only accept MAX_CONN connections
    while (conn_counter < MAX_CONN) {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR){
            perror("tcp_wait_for_connection failed\n"); exit(EXIT_FAILURE);
        }
        // create a thread for each client
        if (pthread_create(&clientthreads[conn_counter], &attr, client_handler, (void*) client) != 0){
            perror("pthread_create failed\n"); exit(EXIT_FAILURE);
        }
        printf("connmgr created thread number = %d \n",conn_counter);
        conn_counter++;
    }

    // wait for target threads to terminate
    while (conn_counter >  0) {
        pthread_join(clientthreads[conn_counter-1],NULL);
        printf("connmgr joined thread number = %d \n",conn_counter);
        conn_counter--;
    }
 
    // mark end_of_stream of sbuffer as true
    sbuffer_set_end(buffer,true);
    
    // tcp close connection fail safe
    if (tcp_close(&server) != TCP_NO_ERROR){exit(EXIT_FAILURE);}
    printf("Test server is shutting down\n");

    // join main thread
    pthread_exit(NULL);
    return NULL;
    
}




