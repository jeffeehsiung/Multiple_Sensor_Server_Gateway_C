#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h" //config.h is included in datamgr.h
#include "lib/dplist.h"
#include <time.h>


#define NO_ERROR 0
#define MEMORY_ERROR 1 // error  mem alloc failure
#define INVALID_ERROR 2 //error list or sensor null

dplist_t* list;
void* element_copy(void*);
void element_free(void**);
int element_compare(void*,void*);

/* sensor element */
typedef struct sensor {
        uint16_t sensor_id;
        uint16_t room_id;
        double running_avg; // computed by datamgr
        time_t last_modified;
        double temperatures[RUN_AVG_LENGTH];
}sensor_t;

/* fscanf to read in full lines, fread to read binary data, fwrtie to write binary data
open, system call. (filename, flags) to read,write,create,append,trucate. returns and int as fileid
close is needed if we've open
fopen is faster than open since it uses buffer
fscan returns 0 if scan is unsuccessful 
fread returns the size num per defined size bytes*/
void datamgr_parse_sensor_files(FILE* fp_sensor_map, FILE* fp_sensor_data){
	printf("length: %d min :%lf max: %lf\n",RUN_AVG_LENGTH,SET_MIN_TEMP,SET_MAX_TEMP);
        list = dpl_create(element_copy,element_free,element_compare); //to hold all sensor nodes
        // read  from file
	fp_sensor_map = fopen("read_sensor.map","r");
	if(!fp_sensor_map){ printf("could not open file \a\n"); exit(101);}
	/* read map, text file*/
	uint16_t roomidBuff;
        uint16_t sensoridBuff;
	// while loop with fscanf to iterate. fileptr, value expected, place to store. 
	// when we get 0 from fscanf, we are done reading information of the file
	int count = 0;
        while(fscanf(fp_sensor_map, "%hd %hd", &roomidBuff, &sensoridBuff)>0) {
                sensor_t* sensor = malloc(sizeof(sensor_t)); //heap, to be freed
		ERROR_HANDLER(sensor == NULL, MEMORY_ERROR)
		sensor->room_id = roomidBuff; 
                sensor->sensor_id = sensoridBuff;
		count++;
		// each sensor constains an array of temp
                for(int i=0; i<RUN_AVG_LENGTH; i++) { 
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
        while(fread(&sensoridBuff, sizeof(sensoridBuffer), 1, fp_sensor_data)>0){
		sensor_t* sensor;
		// get index per sesnor id
                index = dpl_get_index_of_element(list, (void*) &sensoridBuff);
		// get the sensor element
                sensor = (sensor_t*) dpl_get_element_at_index(list,index);
		// load the temp array val from sensor & cal avg
                fread(&temperaturebuffer, sizeof(temperaturebuffer), 1, fp_sensor_data);
                double sum = 0;
		// push out the earilerst temp, sum the history temp, load new temp, compute avg
                for(int i = RUN_AVG_LENGTH-1; i>0; i--)
                {
                        sensor->temperatures[i] = sensor->temperatures[i-1];
                        sum += sensor->temperatures[i];
                }
                sensor->temperatures[0] = temperatureBuff;
                sum += temperaturebuffer;
                sensor->running_avg = sum/RUN_AVG_LENGTH;
		// load timestamp
                fread(&timebuffer, sizeof(timebuffer), 1, fp_sensor_data);
                sensor->last_modified = timebuffer;
		// avg temp abnormal checking
                if(sensor->running_avg != 0 && sensor->running_avg != NULL){
                        if(sensor->running_avg < SET_MIN_TEMP){
				fprintf( stderr,"avg temp < min. room %hu sensor %hu avg %lf on %ld\n",(sensor->room_id),(sensor->sensor_id), sensor->running_avg, sesnor->last_modified);
                        }
                        else if(sensor->running_avg > SET_MAX_TEMP){
                                fprintf( stderr, "avg temp > max. room %hu sensor %hu avg %lf on %ld\n",(sensor->room_id),(sensor->sensor_id),(sensor->running_av)g,(sensor->last_modified));
                        }
                }
		printf("list containing: %d sensors", dpl_size(list));
};

void datamgr_free(){
	dpl_free(&list,true);
};

sensor_t* datamgr_get_sensor_per_sensorid(sensor_id_t sensor_id){
        if(list != NULL){ // list has sensor data
		sensor_t* sensor = NULL;
                for (int i = 0; i < dpl_size(list); i++){
                        if(dpl_get_reference_at_index(list,i)->sensor_id == sensor_id){
                                sensor = dpl_get_reference_at_index(list,i);
                                return sensor
                        }
                }
                ERROR_HANDLER(sensor == NULL,INVALID_ERROR);
        }
        ERROR_HANDLER(list == NULL, INVALID_ERROR);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	if(sensor != NULL){
		sensor->room_id = room_id;
		return room_id;
	}
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	sensor_value_t avg = 0;
	if(sensor != NULL){
		if(sensor->temperatures[RUN_AVG_LENGTH-1] != NULL || sensor->temperatures[RUN_AVG_LENGTH-1] != 0){
		avg = sensor->running_avg;
	}
	return avg;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	sensor_t* sensor = datamgr_get_sensor_per_sensorid(sensor_id);
	time_t time = NULL;
	if(sensor != NULL){
		time = sensor->last_modified;
		return time_t; 
	}
	if(time == NULL){printf("last time modification is 0");}
	return time_t;
}

int datamgr_get_total_sensors(){
        return dpl_size(list);
};



void* element_copy(void* element) {
	if(element == NULL){ return NULL; } // if element is null, no my_element_t will be on heap
    	my_element_t* copy = malloc(sizeof (my_element_t)); // needs to be freed
    	char* new_name;
    	asprintf(&new_name,"%s",((my_element_t*)element)->name); //asprintf requires _GNU_SOURCE
	//allocate a string large enough & return a pointer to it via the first argument & should be freed.
	//char* new_name should be freed
    	assert(copy != NULL);
    	copy->id = ((my_element_t*)element)->id; // deep copy
    	copy->name = new_name;
	free(new_name); // added, should adda breakpoint here at line27
    	return (void*) copy;
}

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
void element_free(void** element) {
    free((((my_element_t*)*element))->name); //type casted, name on heap freed
    free(*element); // free my_element on heap
    *element = NULL;
}

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int element_compare(void* x, void* y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}
