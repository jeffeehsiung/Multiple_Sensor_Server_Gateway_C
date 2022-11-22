#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "config.h"
#include "sensor_db.h"
#include <errno.h>

#define NO_ERROR "no error"
#define MEMORY_ERROR "mem err" // error  mem alloc failure
#define INVALID_ERROR "invalid err" //error list or sensor null
#define FIFO_PATH "myfifo"
#define MAX_BUFF 1024
#define NEWCSV 'a'
#define APPENDCSV 'b'
#define INSERTED 'c'
#define INSERTFAIL 'd'
#define CSVCLOSED 'e'

int fdw;
int sequence = 0;

int writer_create_fifo(char* myfifo){
        fdw = mkfifo(myfifo, 0666);
        if(fdw < 0){
                perror("writer create fifo failed: \t");
                exit(EXIT_FAILURE);
        }
	return fdw;
}

int writer_open_and_write_fifo(FILE* wr_stream, char code){
	int strlength = 0;
        // open fifo for write only
        if(wr_stream != NULL){
                // take input from user and put to STDIN
		time_t t;
		char* msg;
		time(&t);
		asprintf(&msg, "%d%d%s\n", code, sequence, ctime(&t));
                printf("strmgr log message: %s \n",msg);
		// write input on fifo and close it
                write(fdw, msg, strlen(msg)+1);
                close(fdw);
		sequence++;
		strlength = strlen(msg)+1;
        }else{
                printf("strmgr create fifo failed. exit \n");
                exit(0);
        }
	return strlength;
}


FILE* open_db(FILE* wr_stream, char* filename, bool append){
	/* filename of the csv file
	bool: csv file exist, overwritten = false; exist: append = true; */
	FILE* fileptr = fopen(filename, ((append == true)? "a+": "w+"));
	if(append){
		writer_open_and_write_fifo(wr_stream,APPENDCSV);
	}else{writer_open_and_write_fifo(wr_stream,NEWCSV);}
	return fileptr;
}

/* append single sensor reading to the csv file */
int insert_sensor(FILE* wr_stream, FILE* f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	int total_rows = get_total_rows_csv(f,sizeof(sensor_data_t));
	fprintf(f,"%hu,%lf,%ld\n", id, value, ts); // %hu, short unsigned int; time_t, %li
	if (ferror(f)){
		printf("error writing to file, return 0 \n");
		writer_open_and_write_fifo(wr_stream,INSERTFAIL);
		return -1;
	}
        writer_open_and_write_fifo(wr_stream,INSERTED);
	// assume the returned int shall be the newly added row index
	return total_rows;
}

/* close the csv file */
int close_db(FILE* wr_stream, FILE* f){
	fclose(f);
	writer_open_and_write_fifo(wr_stream,CSVCLOSED);
	return 0;
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
int storemgr_parse_sensordata_in_csv(FILE* wr_stream, FILE* openedbinaryfile, FILE* csv){
	int insertrows = 0;
	sensor_data_t* sensordata = malloc(sizeof(sensor_data_t)); // heap, be freed
        ERROR_HANDLER(sensordata == NULL, MEMORY_ERROR);
        while(fread(&(sensordata->id), sizeof(sensordata->id), 1, openedbinaryfile)>0){ 
                fread(&(sensordata->value), sizeof(sensordata->value), 1, openedbinaryfile);
                fread(&(sensordata->ts), sizeof(sensordata->ts), 1, openedbinaryfile);
		insertrows = insert_sensor(wr_stream, csv, sensordata->id, sensordata->value, sensordata->ts);
		if(insertrows == 0){ printf("store manager parsing data failed \n"); return -1; }
        }
	free(sensordata); //heap, freed
	return insertrows;
}

int writer_get_fd(){
	return fdw;
}
