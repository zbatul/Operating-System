/* The following is an example of the use of the sigaction system call. You may need to tweak this a bit to get it to work. */

/* file sigaction.c */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void callme(int sig_num) 
{ 
	 printf("catch signal %d\n", sig_num); 
}



int main(int argc, char * argv[]) 
{

	sigset_t  sigmask;

	struct sigaction  action, old_action;

	sigemptyset(&sigmask);
	if (sigaddset( &sigmask, SIGTERM)==-1 || 
		sigprocmask(SIG_SETMASK, &sigmask, 0)==-1)
          perror("set signal mask");
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask,SIGSEGV);
#ifdef SOLARIS_25
	action.sa_handler = (void (*)(int))callme;
#else
	action.sa_handler = callme;
#endif

	action.sa_flags = 0;
	if (sigaction(SIGINT,&action,&old_action)==-1) 
		perror( "sigaction");
	pause();	/* wait for signal interruption */
        printf("%s exits\n", argv[0]);
	return 0;

}

/* The output of this program should be:

% cc sigaction.c -o sigaction
% sigaction
[1] 415
% kill -INT 415
catch signal: 2
sigaction exits
[1] Done

*/
