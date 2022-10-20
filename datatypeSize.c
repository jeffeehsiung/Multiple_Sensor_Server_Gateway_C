#include <stdio.h>

void charIsSigned() {
	if ((char)(-1)<0)
	{printf("char is signed \n");}
	else{printf("char is unsigned \n");}
	}
int main(){
	int 	a	=100;
	long int	longa	=100;
	short int	shorta	=100;
	int* intptr	=&a;
	long int* longintptr	=&longa;
	short int* shortintptr	=&shorta;
	printf("the size of int data type is: %lu bytes.\n", sizeof(int));
	printf("the size of char data type is: %lu bytes.\n", sizeof(char));
	printf("the size of float data type is: %lu bytes. \n", sizeof(float));
	printf("the size of double data type is: %lu bytes. \n", sizeof(double));
	printf("the size of void data type is: %lu bytes. \n", sizeof(void));
	printf("the size of long int data type is: %lu bytes.\n", sizeof(longa));
	printf("the size of short int data type is: %lu bytes.\n", sizeof(shorta));
	printf("the size of intptr data type is: %lu bytes. \n", sizeof(intptr));
	printf("the size of long int ptr data type is: %lu bytes.\n", sizeof(longintptr));
	printf("the size of short int ptr data type is: %lu bytes.\n", sizeof(shortintptr));
	charIsSigned();
	return 0;
	}
