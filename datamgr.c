#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h" //config.h is included in datamgr.h
#include "lib/dplist.h"
#include <time.h>


#define NO_ERROR "a"
#define MEMORY_ERROR "b" // error  mem alloc failure
#define INVALID_ERROR "c" //error sensor not found

dplist_t* list;
void* element_copy(void*);
void element_free(void**);
int element_compare(void*,void*);

/* sensor node */
typedef struct sensor {
        uint16_t sensor_id;
        uint16_t room_id;
        double running_avg; // computed by datamgr
        time_t last_modified;
        double temperatures[RUN_AVG_LENGTH];
}sensor_t;


/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	printf("length: %d min :%lf max: %lf\n",RUN_AVG_LENGTH,SET_MIN_TEMP,SET_MAX_TEMP);
        list = dpl_create(element_copy,element_free,element_compare);
        uint16_t roombuffer;
        uint16_t sensorbuffer;
        while(fscanf(fp_sensor_map, "%hd %hd", &roombuffer, &sensorbuffer)>0) { //hd means short integer of 2 bytes
                sensor_t * sensor;
                sensor = malloc(sizeof(sensor_t));
                ERROR_HANDLER(sensor==NULL,MEMORY_ERROR);
                sensor->room_id=roombuffer;
                sensor->sensor_id=sensorbuffer;
                for(int i=0; i<RUN_AVG_LENGTH; i++) {
                        sensor->temperatures[i]=0;
                }                          //init values at 0
                dpl_insert_at_index(list,sensor,100,false); //achteraan invoegen
        }
        double temperaturebuffer;
        time_t timebuffer;
        int index;
        while(fread(&sensorbuffer,sizeof(sensorbuffer),1,fp_sensor_data)>0)        //returns number of read files
        {
                sensor_t * sensor;
                index = dpl_get_index_of_element( list, (void *)&sensorbuffer);
                sensor = (sensor_t *)dpl_get_element_at_index(list,index);
                fread(&temperaturebuffer,sizeof(temperaturebuffer),1,fp_sensor_data);
                double running_avgdummy=0;
                for(int i=RUN_AVG_LENGTH-1; i>0; i--)
                {
                        sensor->temperatures[i]=sensor->temperatures[i-1];
                        running_avgdummy += sensor->temperatures[i];
                }
                sensor->temperatures[0]=temperaturebuffer;
                running_avgdummy +=temperaturebuffer;
                sensor->running_avg=running_avgdummy/RUN_AVG_LENGTH;
                fread(&timebuffer,sizeof(timebuffer),1,fp_sensor_data);
                sensor->last_modified = timebuffer;
                if(sensor->temperatures[RUN_AVG_LENGTH-1] != 0) //checken of temperaturen array volledig vol is
                {
                        if(sensor->running_avg<SET_MIN_TEMP)
                        {
                                fprintf( stderr,"It's too cold in room %hu for sensor %hu with a running average of %lf on %ld\n",(sensor->room_id),sensorbuffer,sensor->running_avg,timebuffer );
                        }
                        else if(sensor->running_avg > SET_MAX_TEMP)
                        {
                                fprintf( stderr, "It's too warm in room %hu for sensor %hu with a running average of %lf on %ld\n",(sensor->room_id),sensorbuffer,sensor->running_avg,timebuffer);
                        }
                }list = dpl_create(element_copy,element_free,element_compare);
        uint16_t roombuffer;
        uint16_t sensorbuffer;
};

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id);


/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);


time_t datamgr_get_last_modified(sensor_id_t sensor_id);

int datamgr_get_total_sensors();



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
