#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include "sensor_db.h"
#include "logger.h"
#include "config.h"
#include <time.h>

#define MAX_BUFF 1024

int main(){

	FILE* map = fopen("room_sensor.map", "r");
    	FILE* data = fopen("sensor_data", "rb");

    	if(map == NULL) return -1;
    	if(data == NULL) return -1;

	FILE* csv = NULL;
	FILE* log = NULL;

	pid_t pid;

	int status= 0;
	int insertedrows = 0;

	char* myfifo = "myfifo";
	int strlength1;

	/* message files */
	FILE *rd_stream,*wr_stream;


	/* create fifo*/
	int err = mkfifo(myfifo,0666);
        if(err == -1){
		perror("fail to create fifo");
                exit(EXIT_FAILURE);
        }

	/*for a child*/
	pid = fork();

	if(pid < 0){
		printf("fork failed. \n");
		return -1;
	}

	/*parent process*/
	if(pid > 0){
		/* log file open */
		char* logpath = "gateway.log";
                bool append = true;
                log = reader_open_log(logpath, append);

		/* open read only fifo */
		rd_stream = fopen(myfifo,"r");
        	if(rd_stream == NULL){
                	perror("logger open error by perror \t");
                	exit(EXIT_FAILURE);
        	}

	        /* clear O_NONBLOCK to avoid continuous writing during later scan */
        	/*fcntl(fileno(rd_stream),F_GETFL);
        	int flags = 0;
        	flags |= O_NONBLOCK;
        	fcntl(fileno(rd_stream),F_SETFL,flags);*/

		while(reader_read_and_parse_fifo(rd_stream,log) == 0){
		strlength1 = reader_read_and_parse_fifo(rd_stream,log);}
		wait(NULL);

		printf("reader read string length: %d \n", strlength1);

		/* remove the FIFO */
    		unlink(myfifo);

		if ((WIFEXITED(status) !=0) || (WEXITSTATUS(status) !=0)) {
			printf("\nchild exited with code %d \n",WEXITSTATUS(status));
			exit(EXIT_FAILURE);
		}
	}
	else{
		/* open write only fifo*/
		wr_stream = fopen(myfifo,"w");
                if(wr_stream == NULL){
                        perror("logger open error by perror \t");
                        exit(EXIT_FAILURE);
                }
		/* csv file open & write to wr_stream */
                csv = open_db(wr_stream, "sensordata.csv", true);
		/* insert & write to wr_stream */
                insertedrows = insert_sensor(wr_stream, csv, 1, 20, 10);
                //insertedrows = storemgr_parse_sensordata_in_csv(wr_stream,data,csv);
        	printf("total rows inserted: %d \n", insertedrows);
                /* close csv & write to wr_stream */
		close_db(wr_stream,csv);
		exit(status);
	}

    	fclose(map);
    	fclose(data);

	return 0;


}
