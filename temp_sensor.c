#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define frequency	30
#define min_temp	(-10*100)
#define max_temp	35*100

void read_sensor(){
	double random_temperature = ((min_temp + rand()%(max_temp - min_temp))/100.0);//generates pseudo-random number in  between [0,(max-min)] then add the min temp and divide the vector by 100 so that we get possible decimal temp change
	time_t t = time(NULL);
	printf ( "Temperature = %1.2f @ %s\n",random_temperature,ctime(&t));
    	sleep(frequency);
}

int main(){
	srand (time(NULL)); // srand to initialize the generated psudo random number by rand() function
	while(1) {
		read_sensor();
	}
	return 0;
}
