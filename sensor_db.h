#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "config.h"

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)    do {                       \
                      if (condition) {                              \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE);                         \
                      }                                             \
                    } while(0)

/*
 * Create a test program (main.c) and sensor_db module (the storage manager) to
 * store temperature data into a comma separated value file (csv).
 * the implementation (sensor_db.c) with the following operations:
 * an operation to open a csv file with a given name, and providing an indication
 * if the csv should be overwritten if the file already exists
 * if the data should be appended to the existing csv
 * an operation to append a single sensor reading to the csv file
 * an operation to close the csv file
 */

/* fifo create */
char* writer_create_fifo();

/* open fifo and write to fifo*/
void writer_open_and_write_fifo(char* myfifo, char* message);

/*open fifo and read from fifo*/
void writer_open_and_read_fifo(char* myfifo);


/* open csv file with a given name, providing an indication
 * if the file should be overwritten if the file already exists,
 * if the data should be appended to the existing file,
 * if no file exist then we create a file */
FILE* open_db(char* myfifo, char* filename, bool append);

/* append  single sensor reading to the csv file */
int insert_sensor(char* myfifo,sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/* close the csv file */
int close_db(char* myfifo, FILE* f);

/* calculating the total rows in the csv */
int get_total_rows_csv(FILE* f, int sizeofstruct);

/* read sensor data from binary file fprint into a csv file */
int storemgr_parse_sensordata_in_csv(char* myfifo, FILE* openedbinaryfile);

int writer_get_fd();

#endif /* _SENSOR_DB_H_ */
