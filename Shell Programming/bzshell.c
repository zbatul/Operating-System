/* 
 * Name: Batul Zamin
 * Class: CS421 Operating Systems 
 * Program: Write a C program to create your own shell.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <regex.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough */
#define BUFFER_SIZE 4000 /* maximum characters in history buffer */
#define HISTORY_SIZE 50 /* maximum number of commands that history will hold */

char history_buffer[HISTORY_SIZE][BUFFER_SIZE];
static int hist_count = 0;

/* the signal handler function to print last 10 commands on SIGQUIT */
void handle_SIGQUIT() {
    printf("\nHISTORY: LAST 10 COMMANDS\n");
    int i = 0;
    char buffer[BUFFER_SIZE] = "";
    for(i = hist_count; i >= hist_count - 9 && i >= 1; i--) {
       char ch[MAX_LINE];
       int k = hist_count - i + 1; /* find command number from end */
       /* append command number and history command to buffer */
       sprintf(ch, "%d", k); 
       strcat(ch, ". ");
       strcat(ch, history_buffer[i-1]); 
       strcat(buffer, ch);
    }
    write(STDOUT_FILENO, buffer, strlen(buffer)); /* display buffer */
    return;
}
 
/* Function to add user commands to history_buffer array */
void add_history(char inputBuffer[], int* length) {
    if (inputBuffer[0] != '\n'){
       if(inputBuffer[*length-1] != '\0')
          inputBuffer[*length++] = '\0'; /* append null character at end of string if not null terminated */
       strcpy(history_buffer[hist_count], inputBuffer);
       hist_count++;
    }
}

/* Function to get command back from history for r command */
char* get_history(char inputBuffer[], int length) {
    if(length - 1 == 1) {
	return history_buffer[hist_count - 1];	
    }
    else {
	int num = 0;
	sscanf(inputBuffer, "%*[^0123456789]%d", &num); /* extract number from string */
	if(num > 0 && num <= hist_count)
	   return history_buffer[hist_count - num];
	else
	   return inputBuffer;
    }
}

/* Add inputBuffer to history_buffer if its not empty and not r command 
else process r command, find the original command and add it to history */
void process_command(char inputBuffer[], int* length) {
    regex_t regex;
    int reti;
    reti = regcomp(&regex, "^\\s*r\\s\\s*\\d*\\s*", 0); /* regex compile */
    reti = regexec(&regex, inputBuffer, 0, NULL, 0); /* regex execute */
    /* checking if command is 'r args' with a regex */
    if(!reti) {
       char* rcommand = get_history(inputBuffer, *length); /*get original command from history */
       int l = strlen(rcommand);
       add_history(rcommand, &l); /* add original command to history */
       memset(inputBuffer, 0, MAX_LINE);
       strncpy(inputBuffer, rcommand, l); /* copy original command to inputBuffer */
       *length = strlen(inputBuffer);
       printf("Command : %s",inputBuffer);
    }
    else 
       add_history(inputBuffer, length); /* add original command to history */
    regfree(&regex);
}

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */
void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
   
    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 

    /* process command in inputBuffer and add it to history_buffer */
    process_command(inputBuffer, &length);

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    /* examine every character in the inputBuffer */
    for (i=0;i<length;i++) {
        switch (inputBuffer[i]){
          case ' ':
          case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
          case '\n':                 /* should be the final char examined */
            if (start != -1){
                    args[ct] = &inputBuffer[start];    
                ct++;
            }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
            break;
          default :             /* some other character */
            if (start == -1)
                start = i;
            if (inputBuffer[i] == '&'){
                *background  = 1;
                start = -1;
                inputBuffer[i] = '\0';
            }
          }
     }   
     args[ct] = NULL; /* just in case the input line was > 80 */
}

/* Function for internal command 'whisper' which converts uppercase arguments to lowercase */
void whisper(char** arg) {
    int i = 1, j;
    while(arg[i] != NULL) {
      j = 0;
      while(arg[i][j] != '\0') {
         arg[i][j] = tolower(arg[i][j]);
	 j++;
      }
    i++;
    }
}

/* Function to execute a process by forking a child process */
void execute(char* args[], int background){
    pid_t pid;
    pid = fork();
    /* Error occured */
    if(pid < 0) {
       fprintf(stderr, "Fork failed.\n");
       exit(-1);
    }
    /* Child Process */
    else if(pid == 0) {
       if(execvp(args[0], args) < 0) {
	  fprintf(stderr, "execvp failed.\n");
          exit(-1);
       }
    }
    /* Parent Process */
    else{
       /* command not in background: parent waits for child to complete */
       if(background == 0) {
          printf("[Child pid = %d, Background = FALSE]\n", pid);
          waitpid(pid, NULL, 0);
          printf("Child process complete.\n");
       }
       /* command in background: parent does not wait for child */
       else {
          printf("[Child pid = %d, Background = TRUE]\n", pid);
          return;
       }
    }
}

int main(void)
{
    char inputBuffer[MAX_LINE];  /* buffer to hold the command entered */
    int background;              /* equals 1 if a command is followed by '&' */
    char *args[(MAX_LINE/2)+1];  /* command line (of 80) has max of 40 arguments */

    int cmd_count = 0;
    int pid = getpid();
    printf("Welcome to bzshell. My pid is %d.\n", pid); /* Welcome Message */

    /* set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGQUIT;
    handler.sa_flags = SA_RESTART;
    sigaction(SIGQUIT, &handler, NULL);

    /* wait for <control> <\> */
    /* Program terminates normally inside setup */
    while (1){  
       background = 0;
       printf("bzshell[%d]: ", cmd_count++);
       fflush(stdout);
       setup(inputBuffer, args, &background);       /* get next command */

      /* the steps are:
       (0) if built-in command, handle internally
       (1) if not, fork a child process using fork()
       (2) the child process will invoke execvp()
       (3) if background == 0, the parent will wait,
            otherwise returns to the setup() function. */

       char buff[256];
       if(args[0] != NULL){
          if(strcmp (args[0], "exit") == 0 ) { /* exit command */
	     snprintf(buff, sizeof(buff), "ps -p %d -o pid,ppid,pcpu,pmem,etime,user,command", pid);
	     system(buff);
	     exit(0);
          }
          else if(strcmp (args[0], "whisper") == 0 ) { /* whisper command */
             whisper(args);
             int i = 1;
	     while(args[i] != NULL) {
	        printf("%s ", args[i]);
	        i++;
             }
	     printf("\n");
          }
          else { /* any other command */
             execute(args, background);
          }
       }
    }
    return 0;
}
