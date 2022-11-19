#include <stdio.h>
#include <aio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

/* Develop a C program using ordinary pipes
in which the parent process sends a string message to a child process,
and the child process reverses the case of each character in the message
and then prints it on the command-line.
For example, if the parent process sends the message “Hi There”,
the child process will print “hI tHERE”.
You can use the pipe() command for this*/

#define BUFFER_SIZE	25
#define READ_END	0
#define WRITE_END	1


int main(){

	char write_msg[BUFFER_SIZE] = "Hi There";
	char read_msg[BUFFER_SIZE];

	printf("\n Hello World! \n\t systemcall pipe() = message passing \n");
	pid_t pid;
	int fd[2]; // two ends of a file description for read and write

	/* create the pipe */
	
	/* pipe() creates a pipe, a unidirectional data channel that can be
	 * used for interprocess communication. The array pipefd is used to
	 * return two file descriptors, fd, referring to the ends of the pipe.
         * pipefd[0] refers to the read end of the pipe.  pipefd[1] refers
         * to the write end of the pipe.  Data written to the write end of
         * the pipe is buffered by the kernel until it is read from the read
         * end of the pipe. detail: man 2 pipe*/
	if (pipe(fd) == -1) {
		fprintf(stderr,"Pipe failed");
		return 1;
	}

	/* fork a child process */
    	pid = fork();
    	if (pid < 0){
        	printf("I am the walrus. Fork failed.\n");
        	return -1;
    	}

	/* parent process */
	if (pid > 0){
        	printf("I am the parent! I will write to the child\n");
		/* close the unused end of the pipe */
		close(fd[READ_END]);

		/* write to the pipe */
		write(fd[WRITE_END], write_msg, strlen(write_msg)+1);

		/* close the write end of the pipe */
		close(fd[WRITE_END]);
    	}
	/* child process */
    	else{
        	printf("I am the child!\t");
    		/* close the unused end of the pipe */
		close(fd[WRITE_END]);

		/* read from the pipe */
		read(fd[READ_END], read_msg, BUFFER_SIZE);
		for(int i = 0; i < strlen(read_msg); i++){
			if(islower(read_msg[i])){
				read_msg[i] = toupper(read_msg[i]);
			}else{
				read_msg[i] = tolower(read_msg[i]);
			}
		}
		printf("child read %s\n",read_msg);

		/* close the write end of the pipe */
		close(fd[READ_END]);
	}
    	return 0;
}
