#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h" //config.h is included in datamgr.h
#include "lib/dplist.h"
#include <time.h>


#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null

dplist_t* list;
void* element_copy(void*);
void element_free(void**);
int element_compare(void*,void*);

/* fscanf to read in full lines, fread to read binary data, fwrtie to write binary data
open, system call. (filename, flags) to read,write,create,append,trucate. returns and int as fileid
close is needed if we've open
fopen is faster than open since it uses buffer
fscan returns 0 if scan is unsuccessful
fread returns the size num per defined size bytes*/
void datamgr_parse_sensor_files(FILE* fp_sensor_map, FILE* fp_sensor_data){
	printf("length: %d min :%d max: %d\n",RUN_AVG_LENGTH,SET_MIN_TEMP,SET_MAX_TEMP);
        list = dpl_create(element_copy,element_free,element_compare); //to hold all sensor nodes
	/* read map, text file*/
	uint16_t roomidBuff;
        uint16_t sensoridBuff;
	// while loop with fscanf to iterate. fileptr, value expected, place to store.
	// when we get 0 from fscanf, we are done reading information of the file
	int count = 0;
        while(fscanf(fp_sensor_map, "%hd %hd", &roomidBuff, &sensoridBuff)>0){
                sensor_t* sensor = malloc(sizeof(sensor_t)); //heap, to be freed
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
	/* read sensor data from binary file and configure it with the sensor node in list*/
        double temperatureBuff;
        time_t timeBuff;
        int index;

	// while loop with fread with reading size 1*sensoridBuff and store it in sensoridBuff
        while(fread(&sensoridBuff, sizeof(sensoridBuff), 1, fp_sensor_data)>0){
		sensor_t* sensor;
		// get index per sesnor id
                index = dpl_get_index_of_element(list, (void*) &sensoridBuff);
		// get the sensor element
                sensor = (sensor_t*) dpl_get_element_at_index(list,index);
		// load the temp array val from sensor & cal avg
                fread(&temperatureBuff, sizeof(temperatureBuff), 1, fp_sensor_data);
                double sum = 0;
		// push out the earilerst temp, sum the history temp, load new temp, compute avg
                for(int i = RUN_AVG_LENGTH-1; i>0; i--){
                        sensor->temperatures[i] = sensor->temperatures[i-1];
                        sum += sensor->temperatures[i];
                }
                sensor->temperatures[0] = temperatureBuff;
                sum += temperatureBuff;
                sensor->running_avg = sum/RUN_AVG_LENGTH;
		// load timestamp
                fread(&timeBuff, sizeof(timeBuff), 1, fp_sensor_data);
                sensor->last_modified = timeBuff;
		// avg temp abnormal checking
                if(sensor->running_avg != 0){
                        if(sensor->running_avg < SET_MIN_TEMP){
				fprintf( stderr,"avg temp < min. room %hu sensor %hu avg %lf on %ld\n",(sensor->room_id),(sensor->sensor_id), sensor->running_avg, sensor->last_modified);
                        }
                        else if(sensor->running_avg > SET_MAX_TEMP){
                                fprintf( stderr, "avg temp > max. room %hu sensor %hu avg %lf on %ld\n",(sensor->room_id),(sensor->sensor_id),(sensor->running_avg),(sensor->last_modified));
                        }
                }
		printf("list containing: %d sensors", dpl_size(list));
	}
};

void datamgr_free(){
	dpl_free(&list,true);
};

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
	if(time == 0){printf("last time modification is 0");}
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

