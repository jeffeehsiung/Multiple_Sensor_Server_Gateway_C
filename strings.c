#include <stdio.h>
#include <ctype.h>
#include <limits.h> 
#include <stdint.h>

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
	/* strcmp in c compares two strings char by char: 0 equal, >0 ASC|| left > right,<0 ASC|| left < right */
	int res = strcmp(const char* second, const char* str);
	if(res == 0){
		printf("second name and str are equal. \n");
	}
	else if (res > 0){
		printf("second name is > str. \n");
	}
	else{
		printf("second name is < str. \n");
	}
	return 0;
	}
