#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "config.h"
#include "sensor_db.h"

int main()
{
	printf("Hello World\n");

	FILE* map = fopen("room_sensor.map", "r");
    	FILE* data = fopen("sensor_data", "rb");

    	if(map == NULL) return -1;
    	if(data == NULL) return -1;

    	FILE* csv = open_db("sensordata.csv", true);
    	insert_sensor(csv, 1, 20, 10);
	int insertedrows = storemgr_parse_sensordata_in_csv(data,csv);
	printf("total rows inserted: %d \n", insertedrows);
	close_db(csv);

    	fclose(map);
    	fclose(data);

    	return 0;
}
