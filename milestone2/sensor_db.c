#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_db.h"

#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null

FILE* open_db(char* filename, bool append){
	/* filename of the csv file
	bool: csv file exist, overwritten = false; exist: append = true; */
	FILE* fileptr = fopen(filename, ((append == true)? "a+": "w+"));
	return fileptr;
}

/* append  single sensor reading to the csv file */
int insert_sensor(FILE* f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	int total_rows = get_total_rows_csv(f,sizeof(sensor_data_t));
	fprintf(f,"%hu,%lf,%ld\n", id, value, ts); // %hu, short unsigned int; time_t, %li
	if (ferror(f)){
		printf("error writing to file, return 0 \n");
		return 0;
	}
	// assume the returned int shall be the newly added row index
	return total_rows;
}

/* close the csv file */
int close_db(FILE* f){
	fclose(f);
	return 1; // 1 means true
}

/* calculating the total rows in the csv */
int get_total_rows_csv(FILE* f, int sizeofstruct){
	fseek(f,0,SEEK_END);
	int len = ftell(f); // returns -1L if error
	//fseek(f,0,SEEK_SET); // move the cursor back. not actviated here since append is needed.
	if(len == -1l){ printf("csv length error. set length = 0, return size = 0 \n"); len = 0;}
	int size = len/sizeofstruct;
	return size;
}

/* read sensor data from binary file fprint into a csv file */
int storemgr_parse_sensordata_in_csv(FILE* openedbinaryfile, FILE* csv){
	int insertrows = 0;
	sensor_data_t* sensordata = malloc(sizeof(sensor_data_t)); // heap, be freed
        ERROR_HANDLER(sensordata == NULL, MEMORY_ERROR);
        while(fread(&(sensordata->id), sizeof(sensordata->id), 1, openedbinaryfile)>0){ 
                fread(&(sensordata->value), sizeof(sensordata->value), 1, openedbinaryfile);
                fread(&(sensordata->ts), sizeof(sensordata->ts), 1, openedbinaryfile);
		insertrows = insert_sensor(csv, sensordata->id, sensordata->value, sensordata->ts);
		if(insertrows == 0){ printf("store manager parsing data failed \n"); return -1; }
        }
	free(sensordata); //heap, freed
	return insertrows;
}
