/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program that shows time taken by a program.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 8

int main(int argc, char* argv[])
{        
	time_t start_time, end_time;
	clock_t cpu_start_time, cpu_end_time;
	double time_taken = 0.0, cpu_time_taken;
	
	/* Get command from command line arguments */
	char** command = malloc(MAX_COMMAND_LENGTH * sizeof(char *));
	int i;
    for(i = 1; i < argc; i++){
		command[i-1] = argv[i];
    }
    command[i] = NULL;
    
    /* Get start time */
    start_time = time(NULL);
    cpu_start_time = clock();
    
    /* execute a process (UNIX command) by forking a child process */
    pid_t pid;
    pid = fork();
    
    /* Error occured */
    if(pid < 0) {
   		fprintf(stderr, "Fork failed.\n");
    	exit(-1);
    }
    /* Child Process */
    else if(pid == 0) {
       	if(execvp(command[0], command) < 0) {
	      	fprintf(stderr, "execvp failed.\n");
          	exit(-1);
       }
    }
    /* Parent Process */
    else{
       	/* parent waits for child to complete */
       	printf("[Child pid = %d]\n", pid);
       	waitpid(pid, NULL, 0);
       	printf("Child process complete.\n");
    }
    
    /* Get end time */
    end_time = time(NULL);
   	cpu_end_time = clock();
       	
    /* Time taken by the process (UNIX command) to execute */
    time_taken += (double)(end_time - start_time);
    cpu_time_taken += (double)(cpu_end_time - cpu_start_time) / CLOCKS_PER_SEC;
    printf("Time elapsed during the process is : %f seconds.\n", time_taken);
    printf("CPU time taken by the process is : %f seconds.\n", cpu_time_taken);
    
    return 0;
}
