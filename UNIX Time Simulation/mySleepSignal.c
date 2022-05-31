/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program that simulates the sleep function using sigaction and pause.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 100

static char buffer[BUFFER_SIZE];

/* The signal handler function */
void handle_SIGALRM() {
	write(STDOUT_FILENO, buffer, strlen(buffer));
	exit(0);
}

int main(int argc, char* argv[]) {   
	/* Get sleep time from command line */
	int sleep_time;
	if(argc < 2) {
		printf("Sleep time is not provided. Program is exiting.\n");
		exit(-1);  
	} 
	else {
		sleep_time = atoi(argv[1]);
		if(sleep_time == 0) {
			printf("Slept for 0 seconds.\n");
			return 0;
		}	
	}
	   
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGALRM;
	handler.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &handler, NULL);
	
	/* Message when exiting from sleep */
	strcpy(buffer,"Slept for ");
	strcat(buffer, argv[1]);
	strcat(buffer, " seconds\n");

	/* Sleep for given time */
	alarm(sleep_time);
	pause();

	return 0;
}
