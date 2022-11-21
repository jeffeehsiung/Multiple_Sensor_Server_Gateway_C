#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include "sensor_db.h"
#include "logger.h"
#include "config.h"
#include <time.h>

#define MAX_BUFF 1024

void *writer_piping(FILE* csv);
void *logger_pipping(FILE* log);

int main(){

	FILE* map = fopen("room_sensor.map", "r");
    	FILE* data = fopen("sensor_data", "rb");

    	if(map == NULL) return -1;
    	if(data == NULL) return -1;

	FILE* csv = NULL;
	FILE* log = NULL;

	pid_t pid;
	char* myfifo = "myfifo";
	int status= 10;
	int insertedrows = 0;
	/* message buffer */
	char buffer1[MAX_BUFF];
	char buffer2[MAX_BUFF];
	char* writer_message= buffer1;
	char* reader_message= buffer2;

	/*for a child*/
	pid = fork();

	if(pid < 0){
		printf("fork failed. \n");
		return -1;
	}
	/*parent process*/
	if(pid > 0){
		char* logpath = "gateway.log";
                bool append = true;
		reader_create_fifo(myfifo);
                log = open_log(logpath, append);

		while(WIFEXITED(status) == false || read(reader_get_fd(),reader_message, sizeof(reader_message))>0){
			reader_message = reader_open_and_read_fifo(myfifo, reader_message);
			log  = log_event(myfifo, log, reader_message);
		}
		wait(NULL);
		close_log(log);
		/* remove the FIFO */
    		unlink(myfifo);
	}
	else{
		printf("main.c myfifo before func call: %s \n", myfifo);
                writer_create_fifo(myfifo);
                printf("main.c myfifo after func call: %s \n", myfifo);
                csv = open_db(myfifo, writer_message, "sensordata.csv", true); //write to myfifo
                //insertedrows = insert_sensor(myfifo, writer_message, csv, 1, 20, 10); //write to myfifo
                insertedrows = storemgr_parse_sensordata_in_csv(myfifo, writer_message, data,csv);
        	printf("total rows inserted: %d \n", insertedrows);
		close_db(myfifo, writer_message, csv);
		exit(status);
	}

    	fclose(map);
    	fclose(data);

	return 0;


}
