/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program that responds to different signals.
 * Run using compilation command: gcc -o sig sig.c
 * To terminate the process, use command: kill pid (where pid is printed in first line in the program) in another terminal
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAX_LINE 500 /* maximum number of characters on one line of input */
#define HISTORY_SIZE 100 /* maximum number of inputs that history will hold */

/* global variables for storing history */
static int hist_count = 0;
char history_buffer[HISTORY_SIZE][MAX_LINE];
/* global variables for storing alarm time */
int tick = 0;
char time_buff[20];
/* global variable for start and end time of program */
time_t start_time, end_time;


/* Function to print user input history */
void print_history() {
    printf("\nHISTORY: LAST 10 USER INPUTS\n");
    int i = 0;
    char buffer[MAX_LINE * HISTORY_SIZE] = "";
    for(i = hist_count; i >= hist_count - 9 && i >= 1; i--) {
       char ch[MAX_LINE];
       int k = hist_count - i + 1; /* find input number from end */
       /* append input number and history input to buffer */
       sprintf(ch, "%d", k); 
       strcat(ch, ". ");
       strcat(ch, history_buffer[i-1]); 
       strcat(buffer, ch);
    }
    write(STDOUT_FILENO, buffer, strlen(buffer)); /* display buffer */
    printf("\n");
    return;
}

/* Function to add user input to history_buffer array */
void add_history(char inputBuffer[], int* length) {
    if (inputBuffer[0] != '\n') {
       if(inputBuffer[*length-1] != '\0')
          inputBuffer[*length++] = '\0'; /* append null character at end of string if not null terminated */
       strcpy(history_buffer[hist_count], inputBuffer);
       hist_count++;
    }
}

/* The signal handler function */
void handle_SIGNALS(int sig) {
	double time_taken;
	switch (sig) {
		case SIGALRM:
			/* Repititive alarm: sleep for 10 seconds again */
			alarm(10);
			snprintf(time_buff, 20, "\nTick %d...\n\n", tick);
			puts(time_buff);
			tick += 10;
			break;
			
		case SIGINT:
			/* Get end time, calculate time taken by program, print summary */
			end_time = time(NULL);
			time_taken = (double)(end_time - start_time);
			printf("\nTime spent by the program is : %f seconds.\n\n", time_taken);
			break;
			
		case SIGTERM:
			/* Get end time, calculate time taken by program, print summary, exit the program */
			end_time = time(NULL);
			time_taken = (double)(end_time - start_time);
			printf("\nTime spent by the program is : %f seconds.\n", time_taken);
			printf("Exiting the program...\n");
			exit(1);
			
		case SIGTSTP:
			/* Print the last 10 user inputs from history buffer */
			print_history();
			break;
	}
}

/* Main function */
int main(int argc, char* argv[]) {  
	/* get process id to kill the process when done using SIGTSTP */
	pid_t pgId = getpid();
	printf("\nProcess id : %d", pgId);
	
	/* Get start time */
	start_time = time(NULL);
	
	char user_input[MAX_LINE];
	int length = 0;
	
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGNALS;
	handler.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &handler, NULL);
	sigaction(SIGINT, &handler, NULL);
	sigaction(SIGTERM, &handler, NULL);
	sigaction(SIGTSTP, &handler, NULL);
	
	/* Sleep for 10 seconds */
	snprintf(time_buff, 20, "\nTick %d...\n\n", tick);
	puts(time_buff);
	tick += 10;
	alarm(10);
	
	while(1){
		/* read what the user enters on the command line */
    	length = read(STDIN_FILENO, user_input, MAX_LINE);
    	/* add the input to history */
    	add_history(user_input, &length);
    	/* echo user input */
		printf("%s\n", user_input); 
		/* reset string to 0's */
		memset(user_input, 0, strlen(user_input));
	}
	   
	return 0;
}
