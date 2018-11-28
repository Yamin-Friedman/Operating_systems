// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"


/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num) {
	if(fg_pid == 0)
		return;
	if(kill(fg_pid,SIGINT) == -1){
		perror("kill SIGINT:");
		return;
	}
	printf("signal SIGINT was sent to pid %d\n",fg_pid);
}

void handler_cntlz(int sig_num) {
	if(fg_pid == 0)
		return;
	if(kill(fg_pid,SIGTSTP) == -1){
		perror("kill SIGTSTP:");
		return;
	}
	printf("signal SIGTSTP was sent to pid %d\n",fg_pid);
}

