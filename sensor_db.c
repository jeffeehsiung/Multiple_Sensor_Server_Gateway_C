/**
 * \author {Jeffee Hsiung}
 */

#include <stdio.h>
#include <stdlib.h>
#include "sensor_db.h"
#include "sbuffer.h"

#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null

extern int fd[2];
extern sem_t pipe_lock;

void* sensor_db_start(void* param){

	// typecast the param into sbufer_t
	sbuffer_t* buffer = (sbuffer_t*) param;

	// open the csv file
	char* filename = "data.csv";
	FILE* csv = fopen(filename,"a+");
	if(csv != NULL){
		char buf[BUFF_SIZE];
		sprintf(buf, "A new data.csv file has been created\n");
		//lock the semaphore of pipe access
		if (sem_wait(&pipe_lock) == -1){
			perror("storagemgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
		}
		if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
			perror("storagemgr: write to pipe failed\n"); exit(EXIT_FAILURE);
		}
		//unlock the semaphore of pipe access
		if (sem_post(&pipe_lock) == -1){
			perror("storagemgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
		}
	}else{
		perror("storagemgr: sensor_db_open failed\n"); exit(EXIT_FAILURE);
	}

	// read the sensor data from the buffer
	sensor_data_t data;
	int code = SBUFFER_SUCCESS;
	int count = 0;

	// loop until the buffer is empty
	while (code != SBUFFER_END)
	{
		code = sbuffer_remove(buffer, &data,CONSUMER_B);
		if(code == SBUFFER_SUCCESS){
			count++;
		}

		// if the data has been read by consumer B, skip this iteration
		if(code == SBUFFER_NO_DATA){
			continue;
		}else if(code == SBUFFER_FAILURE){
			perror("storagemgr: sbuffer_remove failed\n"); exit(EXIT_FAILURE);
		}

		// write the data into the csv file
		if(fprintf(csv,"%hu,%lf,%ld\n", data.id, data.value, data.ts) < 0){
			perror("storagemgr: fprintf failed\n"); exit(EXIT_FAILURE);
		}else{
			char buf[BUFF_SIZE];
			sprintf(buf, "Data insertion from sensor %hu succeeded.\n", data.id);
			//lock the semaphore of pipe access
			if (sem_wait(&pipe_lock) == -1){
				perror("storagemgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
			if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
				perror("storagemgr: write to pipe failed\n"); exit(EXIT_FAILURE);
			}
			//unlock the semaphore of pipe access
			if (sem_post(&pipe_lock) == -1){
				perror("storagemgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
		}
	}

	printf("storagemgr: end of stream marker detected. total sensor data stored: %d\n", count);

	// close the csv file
	if(fclose(csv) != 0){
		perror("storagemgr: fclose failed\n"); exit(EXIT_FAILURE);
	}else{
		char buf[BUFF_SIZE];
		sprintf(buf, "The data.csv has been closed.\n");
		//lock the semaphore of pipe access
		if (sem_wait(&pipe_lock) == -1){
			perror("storagemgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
		}
		if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
			perror("storagemgr: write to pipe failed\n"); exit(EXIT_FAILURE);
		}
		//unlock the semaphore of pipe access
		if (sem_post(&pipe_lock) == -1){
			perror("storagemgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
		}
	}

	// terminate the thread, release the resources
	pthread_exit(NULL);
	return NULL;
}