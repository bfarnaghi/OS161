#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void) {

	printf("Test: fork, open, dup2, close, getpid, exit, write: \n");

    int pid = fork(); 

	if(pid==0){
		//open a file for the STDOUT file discription in the child process
		int fd = open("file_test.txt", O_WRONLY);
		// change the file descriptor
		dup2(fd, STDOUT_FILENO);
		// Empty the old file descriptor
		close(fd);
		//This function should be euted in a text file
		printf("Child, PID:%d\n",getpid());
		//Exit the chile process and continue the parent process
		exit(4);
	}
	else if(pid>0){
		//Begining of the parent process
		printf("Parent, PID:%d\n",getpid());
		//wait for the child
		int status;
        int pid_1 = waitpid(pid, &status, 0);
		//Print the recieved data from the child
		printf("Parent, status: %d   pid_1:%d    pid:%d\n",status,getpid(),pid_1);
		exit(0);
	}
    return 0;
}
