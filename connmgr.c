/**
 * \author {Jeffee Hsiung}
 */
#include <stdio.h>
#include <stdlib.h>
#include "connmgr.h"
#include "config.h"

#define MAX_CONN 3  // state the max. number of connections the server will handle before exiting

// initialize global variables
tcpsock_t *server,*client;
extern fd;

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
        
        conncounter++;
        // prints to stdout (console)
        if ((result == TCP_NO_ERROR) && bytes) {
            // write to pipe
            if(conncounter == 1){
                char buf[100];
                sprintf(buf,"Sensor node %d has opened a new connection\n",data.id);
                write(fd[WRITE_END],buf,sizeof(buf));
            }
        }
    } while (result == TCP_NO_ERROR);
    
    if (result == TCP_CONNECTION_CLOSED){
        printf("Peer has closed connection\n");
        // write to pipe
        char buf[100];
        sprintf(buf,"Sensor node %d has closed the connection\n",data.id);
        write(fd[WRITE_END],buf,sizeof(buf));
    }
    else{printf("Error occured on connection to peer\n");}

    tcp_close(&client);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

/**
 * Implements a sequential test server (only one connection at the same time)
 */
void connmgr_start(int server_port) {
    
    /* initialize variables */
    int conn_counter = 0;

    pthread_t clientthreads[MAX_CONN];


    /* open tcp connection */
    printf("Test server started\n");
    if (tcp_passive_open(&server, server_port) != TCP_NO_ERROR){exit(EXIT_FAILURE);}


    /* wait for each client and create thread for each client */
    do {
        // accept one client connection. on success, new client socket created
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR){
            exit(EXIT_FAILURE);
        }
        printf("Incoming client connection\n");

        // create client thread with socket number & start the runner + increment the conn_counter
        if (pthread_create(&clientthreads[conn_counter],NULL,client_handler,client) != 0){
            printf("failed to create thread \n");
        }
        conn_counter++;
    } while (conn_counter < MAX_CONN);
    
    /* wait for target threads to terminate */
    while (conn_counter >  0) {
        while (conn_counter != 0){
            // join all threads
            pthread_join(clientthreads[conn_counter],NULL);
            conn_counter--;
        }
    }

    /* tcp close connection fail safe */
    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");

    pthread_detach(pthread_self());
    pthread_exit(NULL);
}




