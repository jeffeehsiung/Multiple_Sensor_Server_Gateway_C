/*
 * \author Jeffee Hsiung
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "config.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 20
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 10
#endif

/* sensor element */
typedef struct sensor {
        uint16_t sensor_id;
        uint16_t room_id;
        double running_avg; // computed by datamgr
        time_t last_modified;
        double temperatures[RUN_AVG_LENGTH];
}sensor_t;

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)    do {                       \
                      if (condition) {                              \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE);                         \
                      }                                             \
                    } while(0)

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void* datamgr_start(void* param);

/**element wise operations*/
void* element_copy(void* element);
void element_free(void** element);
int element_compare(void* x, void* y);
#endif  /* DATAMGR_H_ */
