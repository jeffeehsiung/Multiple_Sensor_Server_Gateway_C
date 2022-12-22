#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "datamgr.h"
#include "sbuffer.h"
#include "lib/dplist.h"
#include <time.h>


#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null

extern int fd[2];
extern sem_t pipe_lock;

dplist_t* list;


void* datamgr_start(void* param){

	// typecast the param into sbufer_t
	sbuffer_t* buffer = (sbuffer_t*) param;

	// open the map file
	FILE* map = fopen("room_sensor.map", "r");
	if (map == NULL){
        perror("map opening file failed\n"); exit(EXIT_FAILURE);
    }

    list = dpl_create(element_copy,element_free,element_compare);

	uint16_t roomidBuff;
    uint16_t sensoridBuff;

	// read sensor map from file and configure it with the sensor node in list
	int count = 0;
	while(fscanf(map, "%hu %hu", &roomidBuff, &sensoridBuff)>0){

		sensor_t* sensor = malloc(sizeof(sensor_t)); //element on heap, to be freed by element free
		ERROR_HANDLER(sensor == NULL, MEMORY_ERROR);
		sensor->room_id = roomidBuff;
		sensor->sensor_id = sensoridBuff;
		// each sensor constains an array of temp
		for(int i=0; i<RUN_AVG_LENGTH; i++){
				sensor->temperatures[i] = 0;
		}
		// for each sensor node we insert into the newly created list and no deep copy to keep on pointing to the heap addr
		list = dpl_insert_at_index(list, sensor, count, false);
		count++;

	}

	// close the map file
	fclose(map);
	
	// for each sensor node, read sensor data from shared buffer and update the sensor node
	sensor_data_t data;
	int code = SBUFFER_SUCCESS;
	count = 0;

	// loop until the end-of-stream marker is detected
	while(code != SBUFFER_END){
		
		code = sbuffer_remove(buffer,&data,CONSUMER_A);
		if(code == SBUFFER_SUCCESS){
			count++;
		}

		// if the data has been read by consumer A, skip this iteration
		if(code == SBUFFER_NO_DATA){
			continue;
		}else if(code == SBUFFER_FAILURE){
			perror("datamgr: sbuffer_remove failed\n"); exit(EXIT_FAILURE);
		}

		// get index of the dplist sensor_t element per grabbed sensor data id. skip if not found
		int index = dpl_get_index_of_element(list, (void*) &(data));
		if(index == -1){
			char buf[BUFF_SIZE];
			//initialize the buffer
            memset(buf,0,BUFF_SIZE);
			sprintf(buf, "Received sensor data with invalid sensor node ID %hu\n",data.id);
			//lock the semaphore of pipe access
			if (sem_wait(&pipe_lock) == -1){
				perror("datamgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
			if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
				perror("datamgr: write to pipe failed\n"); exit(EXIT_FAILURE);
			}
			//unlock the semaphore of pipe access
			if (sem_post(&pipe_lock) == -1){
				perror("datamgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
			continue;
		}

		// get the sensor element in the list
		sensor_t* sensor = (sensor_t*) dpl_get_element_at_index(list,index);
		if(sensor == NULL){
			perror("datamgr: sensor element not found per index\n"); exit(EXIT_FAILURE);
		}

		// load the temp array val from sensor & cal avg
		double sum = 0;

		// push out the earilerst temp, sum the history temp, load new temp, compute avg
		for(int i = RUN_AVG_LENGTH-1; i>0; i--){
			sensor->temperatures[i] = sensor->temperatures[i-1];
			sum += sensor->temperatures[i];
		}

		// load new temp
		sensor->temperatures[0] = data.value;
		sum += data.value;
		sensor->running_avg = sum/RUN_AVG_LENGTH;

		// load timestamp
		sensor->last_modified = data.ts;
		
		// avg temp abnormal checking
		if(sensor->running_avg != 0){
			// write the sensor data to pipe
			char buf[BUFF_SIZE];
			//initialize the buffer
            memset(buf,0,BUFF_SIZE);
			if(sensor->running_avg < SET_MIN_TEMP){
				sprintf(buf, "Sensor node %hu reports it's too cold (avg temp = %lf)\n",(sensor->sensor_id),(sensor->running_avg));
			}else if(sensor->running_avg > SET_MAX_TEMP){
				sprintf(buf, "Sensor node %hu reports it's too hot (avg temp = %lf)\n",(sensor->sensor_id),(sensor->running_avg));
			}
			//lock the semaphore of pipe access
			if (sem_wait(&pipe_lock) == -1){
				perror("datamgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
			if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
				perror("datamgr: write to pipe failed\n"); exit(EXIT_FAILURE);
			}
			//unlock the semaphore of pipe access
			if (sem_post(&pipe_lock) == -1){
				perror("datamgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
			}
		}
	}

	printf("datamgr: end of stream marker detected. total sensor processed: %d\n",count);

	// free the list
	dpl_free(&list, true);

	pthread_exit(NULL);
	return NULL;
}

void* element_copy(void* element) {
	if(element == NULL){ return NULL; } // if element is null, no my_element_t will be on heap
	sensor_t* copy = malloc(sizeof (sensor_t)); // needs to be freed
	sensor_t* sensor = (sensor_t*)element;
	copy->sensor_id = sensor->sensor_id; // \deep copy
	copy->room_id = sensor->room_id;
	copy->running_avg = sensor->running_avg;
	copy->last_modified = sensor->last_modified;
	for(int i = 0; i < RUN_AVG_LENGTH; i++){
		copy->temperatures[i] = sensor->temperatures[i];
	}
    return (void*) copy;
}

void element_free(void** element) {
	sensor_t* sensor = (sensor_t*)*element;
	free(sensor); //type casted, free the sensor_t* on heap
	*element = NULL;
}

int element_compare(void* x, void* y) {
	// x is an element in the list of type sensor_t*
	// y is an element recieved from sbuffer of type sensor_data_t*
	// compare the elememnt by sensor_id
	sensor_t* sensor = (sensor_t*)x;
	sensor_data_t* data = (sensor_data_t*)y;
	if(sensor->sensor_id == data->id){
		return 0;
	}
	return 1;
}

