#include <stdio.h>
#include <ctype.h>
#include <limits.h> 
#include <stdint.h>
#include <string.h>
int strcmp(const char* str1, const char*str2);
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
	return 0;}
