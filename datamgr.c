#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "sbuffer.h"
#include "lib/dplist.h"
#include <time.h>


#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null

extern int fd[2];
extern sem_t pipe_lock;
extern sbuffer_t* buffer;

dplist_t* list;
void* element_copy(void*);
void element_free(void**);
int element_compare(void*,void*);


void* datamgr_parse_sensor_files(void* param){
	
	FILE* fp_sensor_map = (FILE*) param;

    list = dpl_create(element_copy,element_free,element_compare);

	uint16_t roomidBuff;
    uint16_t sensoridBuff;

	// read sensor map from text file and configure it with the sensor node in list
	int count = 0;
	while(fscanf(fp_sensor_map, "%hd %hd", &roomidBuff, &sensoridBuff)>0){
		sensor_t* sensor = malloc(sizeof(sensor_t)); //element on heap, to be freed by element free
		ERROR_HANDLER(sensor == NULL, MEMORY_ERROR);
		sensor->room_id = roomidBuff;
		sensor->sensor_id = sensoridBuff;
		count++;
		// each sensor constains an array of temp
		for(int i=0; i<RUN_AVG_LENGTH; i++){
				sensor->temperatures[i] = 0;
		}
		// for each sensor node we insert into the newly created list and no deep copy to keep on pointing to the heap addr
		dpl_insert_at_index(list, sensor, count, false);
	}

	// for each sensor node, read sensor data from shared buffer and update the sensor node
	sensor_data_t data;
	int code = SBUFFER_SUCCESS;

	// loop until the end-of-stream marker is detected
	while(code != SBUFFER_END){
		code = sbuffer_remove(buffer,&data, CONSUMER_A);
		// if the data has been read by consumer A, skip this iteration
		if(code == SBUFFER_NO_DATA){
			continue;
		}else if(code == SBUFFER_FAILURE){
			perror("datamgr: sbuffer_remove failed\n"); exit(EXIT_FAILURE);
		}
		// get index of the dplist sensor_t element per grabbed sensor data id
		int index = dpl_get_index_of_element(list, (void*) (&data.id));
		// get the sensor element in the list
		sensor_t* sensor = (sensor_t*) dpl_get_element_at_index(list,index);
		if(sensor == NULL){
			perror("datamgr: sensor not found\n"); exit(EXIT_FAILURE);
		}
		// compare the grabbed id with the sensor id in the list
		if(sensor->sensor_id == data.id){
			// set the read_by_a flag for the node in sbuffer to true
        	buffer->head->read_by_a = true;
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
			sensor->timestamp = data.ts;
			// avg temp abnormal checking
			if(sensor->running_avg != 0){
				// lock the semaphore of pipe access
				if (sem_wait(&pipe_lock) == -1){
					perror("datamgr: sem_wait pipe_lock failed\n"); exit(EXIT_FAILURE);
				}
				// write the sensor data to pipe
				char buf[BUFF_SIZE];
				if(sensor->running_avg < SET_MIN_TEMP){
					sprintf(buf, "Sensor node %hu reports it's too cold (avg temp = %lf)\n",(sensor->sensor_id),(sensor->running_avg));
				}else if(sensor->running_avg > SET_MAX_TEMP){
					sprintf(buf, "Sensor node %hu reports it's too hot (avg temp = %lf)\n",(sensor->sensor_id),(sensor->running_avg));
				}
				if (write(fd[WRITE_END], buf, sizeof(buf)) == -1){
					perror("datamgr: write to pipe failed\n"); exit(EXIT_FAILURE);
				}
				// unlock the semaphore of pipe access
				if (sem_post(&pipe_lock) == -1){
					perror("datamgr: sem_post pipe_lock failed\n"); exit(EXIT_FAILURE);
				}
			}
		}
	}

	pthread_exit(NULL);
}

void datamgr_free(){
	dpl_free(&list,true);
}

sensor_t* datamgr_get_sensor_per_sensorid(sensor_id_t sensor_id){
	sensor_t* sensor = NULL;
	if(list != NULL){ // list has sensor data
		for (int i = 0; i < dpl_size(list); i++){
			if(((sensor_t*)dpl_get_element_at_index(list,i))->sensor_id == sensor_id){
					sensor = (sensor_t*) dpl_get_element_at_index(list,i);
					return sensor;
			}
		}
		ERROR_HANDLER(sensor == NULL,INVALID_ERROR);
	}
	ERROR_HANDLER(list == NULL, INVALID_ERROR);
	return NULL;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	if(sensor != NULL){
		return sensor->room_id;
	}
	return 0;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	sensor_value_t avg = 0;
	if(sensor != NULL){
		if(sensor->temperatures[RUN_AVG_LENGTH-1] != 0){
		avg = sensor->running_avg;
		}
	}
	return avg;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	time_t time = 0;
	if(sensor != NULL){
		time = sensor->last_modified;
		return time;
	}
	if(time == 0){perror("datamgr: last time modification is 0\n");exit(EXIT_FAILURE);}
	return time;
}

int datamgr_get_total_sensors(){
        return dpl_size(list);
};

void* element_copy(void* element) {
	if(element == NULL){ return NULL; } // if element is null, no my_element_t will be on heap
	sensor_t* copy = malloc(sizeof (sensor_t*)); // needs to be freed
	sensor_t* sensor = (sensor_t*)element;
	ERROR_HANDLER(copy != NULL,MEMORY_ERROR);
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
	return ((((sensor_t*)x)->room_id != ((sensor_t*)y)->room_id || ((sensor_t*)x)->sensor_id < ((sensor_t*)y)->sensor_id) ? -1 : (((sensor_t*)x)->sensor_id == ((sensor_t*)y)->sensor_id) ? 0 : 1);
}

