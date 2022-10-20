#include <stdio.h>
#include <ctype.h>

int main(){
	char	first[];
	char	second[];
	char	name[];
	char	str[];
	printf("type your first name: ");
	scanf("%s", &first);
	printf("type your second name: ");
	scanf("%s", &second);
	/*convert the second name to all upper case and store result in str*/
	int i = 0;
	while(second[i]){
		str[i]=putchar(toupper(second[i]));
		i++;
		}
	printf("your second name is: %s", str);
	return 0;
	}
