#include <stdio.h>
#include <aio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    	printf("Hello World!\n\n");
    	pid_t pid;
    	int pcfd[2];

    	pid = fork();
    	if (pid < 0){
        	printf("I am the walrus. Fork failed.");
        	return -1;
    	}
    	if (pid > 0){
        	printf("I am the parent!\n\n");
        	wait(NULL);
    	}
    	else{
        	printf("\tI am the child!\n\n");
    	}
	printf("Hello World\n");

	FILE* map = fopen("room_sensor.map", "r");
    	FILE* data = fopen("sensor_data", "rb");

    	if(map == NULL) return -1;
    	if(data == NULL) return -1;

    	datamgr_parse_sensor_files(map, data);

    	datamgr_free();

    	fclose(map);
    	fclose(data);

    	return 0;
}
