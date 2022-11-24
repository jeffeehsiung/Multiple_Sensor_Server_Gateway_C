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

int main(){

	FILE* map = fopen("room_sensor.map", "r");
    	FILE* data = fopen("sensor_data", "rb");

    	if(map == NULL) return -1;
    	if(data == NULL) return -1;

	pid_t pid;
	char* myfifo = "myfifo";
	int status= 10;
	int insertedrows = 0;

	/*fork a child*/
	pid = fork();

	if(pid < 0){
		printf("fork failed. \n");
		return -1;
	}
	/*parent process*/
	if(pid > 0){
		reader_create_fifo(myfifo);
		while((WIFEXITED(status)!=true ||  WEXITSTATUS(status)!=status)){
			reader_open_and_read_fifo(myfifo);
		}
		waitpid(pid,&status, 0);
		/* remove the FIFO */
    		unlink(myfifo);
	}
	else{
                writer_create_fifo(myfifo);
                insertedrows = storemgr_parse_sensordata_in_csv(myfifo,data);
        	printf("total rows inserted: %d \n", insertedrows);
		exit(status);
	}
    	fclose(map);
    	fclose(data);

	return 0;


}
