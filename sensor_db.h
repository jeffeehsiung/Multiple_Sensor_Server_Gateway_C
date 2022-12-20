#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
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

void* sensor_db_start(void* param);

#endif /* _SENSOR_DB_H_ */
