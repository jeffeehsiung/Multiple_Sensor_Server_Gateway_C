#include <stdio.h>
#include <stdlib.h>

typedef struct{
	short day, month;
	unsigned year;
}date_t;

/*function date_struct*/
date_t* date_struct(int day, int month, int year){
	//taking pointer poitning to date_t
	date_t dummy; //local varirable in stack with function lifespan
	date_t* dummy_ptr = (date_t*)malloc(sizeof(date_t));
	dummy.day = (short)day;
	dummy.month = (short)month;
	dummy.year = (unsigned)year;
	printf("dummy.day: %d \n", dummy.day);
	printf("dummy.month: %d \n", dummy.month);
	printf("dummy.year: %d \n", dummy.year);
	*dummy_ptr = dummy;
	//(dummy_ptr->day) = dummy.day;
	//(dummy_ptr->month)=dummy.month;
	//(dummy_ptr->year)=dummy.year;
	return dummy_ptr;
}

/*main*/
int main(void){
	int day, month, year;
	date_t* d;
	printf("\nGive day, month, year. no comma, just number and space:");
	scanf("%d %d %d", &day, &month, &year); // & for integer scanf
	d = date_struct(day, month, year); //pointer with content of dummys
	printf("\ndate struct values: %d-%d-%d \n", d->day, d->month, d->year);
	/* should not work since in the end the pointer in the function call points to dummy, a local function varaible that disappear after function call */
	free(d);
	return 0;
}


