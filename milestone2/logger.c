#include <stdio.h>
#include <aio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>


#define BUFFER_SIZE     25
#define READ_END        0
#define WRITE_END	1

char write_msg[BUFFER_SIZE];
char read_msg[BUFFER_SIZE];

pid_t pid;
int fd[2]; // two ends of a file description for read and write

/* create the pipe */
if (pipe(fd) == -1) {
fprintf(stderr,"Pipe failed");
return 1;
}

/* log event process */
void logger_read_from_pipe(int* pipe, char* logevent, int* buffersize){
	/* read from the pipe */
	read_msg = read(*pipe, *logevent,*buffersize);
	for(int i = 0; i < strlen(read_msg); i++){
        if(islower(read_msg[i])){
                                read_msg[i] = toupper(read_msg[i]);
                        }else{
                                read_msg[i] = tolower(read_msg[i]);
                        }
                }
                printf("child read %s\n",read_msg);

                /* close the write end of the pipe */
                close(fd[WRITE_END]);
}
