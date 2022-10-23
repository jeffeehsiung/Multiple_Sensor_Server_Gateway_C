#include <stdio.h>
#include <ctype.h>
#include <limits.h> 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
char* str_append(char* s1, char* s2); /* return a ptr, take two str as arg, allocate space in heap for new str and space engouh for both of them */

int main(){
	char	first[MB_LEN_MAX];
	char	second[MB_LEN_MAX];
	char	name[MB_LEN_MAX];
	char	str[MB_LEN_MAX];
	printf("type your first name: ");
	scanf("%s", first);
	printf("type your second name: ");
	scanf("%s", second);
	printf("the maximum number of bytes in a multibyte char: %d \n", MB_LEN_MAX);
	/*convert the second name to all upper case and store result in str*/
	int i = 0;
	while(second[i]){
		str[i]=toupper(second[i]);
		i++;}
	printf("your second name is: %s \n", str);
	/* strcmp in c compares two strings char by char in a case sensitive way: 0 equal, >0 ASC|| left > right,<0 ASC|| left < right */
	int res = strcmp(second,str);
	if(res == 0){
		printf("second name and str are equal in case sensitive. \n");
	}
	else if (res > 0){
		printf("second name is > str in ASCII(case). snd to str result: %d \n", res);
	}
	else{
		printf("second name is < str in ASCII(case). snd to str result: %d \n", res);
	}
	/* strcasecmp() is similar to strcmp() but in a ccase insensitive way */
	res = strcasecmp(second,str);
	if(res == 0){
		printf("second and str are equal, case insensitive. \n");
	}
	else if (res > 0){
		printf("second > str in ASCII(no case). result: %d \n", res);
	}
	else{
		printf("second < str in ASCII(no case). result: %d \n", res);
	}

	/* concatenation */
	char* s = str_append(first,second);
	sprintf(name, "%s", s); /*print the result into a buffer array terminal*/
	printf("name: %s \n", name);
	free(s);
	/* read  into int and concatenate with string*/
	printf("enter your birth year: ");
	int year;
	scanf("%d", &year); /*store into  year according to the addr, & is needed for int, char, float, double*/
	int size = strlen(name) + sizeof(year) + 2;
	char name_cpy[strlen(name)+1];
	strncpy(name_cpy,name,strlen(name)+1);
	char buffer[size];
	snprintf(buffer, size, "%s %d\n", name_cpy,year);
	puts(buffer);
	/* read from a string and split */
	char str1[sizeof(buffer)];
	char str2[sizeof(buffer)];
	int year2; 
	sscanf(buffer, "%s %s %d", str1, str2, &year2 );
	printf("first name: %s, second name: %s, year: %d \n", str1, str2, year2);
	return 0;}

char* str_append(char* s1, char* s2)
{
	//make space for new string on the heap that can store str large enough to contain s1 and s2
	int s1_length = strlen(s1);
	int s2_length = strlen(s2);
	int size = s1_length + s2_length + 2;  /* space and  null terminator included */
	char* s =  calloc(size, sizeof(char)); /* go get space on the heap for sth of this size where each element is the size of  sth */
	//copying into s
	for (int i = 0; i < s1_length; i++){
	s[i] = s1[i]; /* copying data without including the null terminator*/
	}
	s[s1_length]= ' '; /* put a space */
	for (int i = 0; i < s2_length; i++){
	s[s1_length +1 + i] = s2[i]; /* shifting over by s1 length and a space */
	}
	// put a null terminator
	s[size -1] = '\0';
	return s; /* ptr to s  */
	// to free s in the heap in the main function call
}
