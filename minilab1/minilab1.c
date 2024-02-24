#include <stdio.h>
#include <unistd.h>

int main(){
	pid_t child = fork();

	if (child < 0){
		printf("Fork failed\n");
		return 1;
	} else if (child == 0) {
		pid_t child_pid = getpid();
		printf("The child process' pid is %d\n", child_pid);
		pid_t parent_pid = getppid();
		printf("The parent process pid is %d\n", parent_pid);
	}

	return 0; 
}
