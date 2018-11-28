// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"

void send_signal(int signal,int pid){
	if(kill(pid,signal) == -1){
		perror("kill:");
		return;
	}
	printf("signal %d was sent to pid %d\n",signal,pid);
}

/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num) {
	if(fg_pid == 0)
		return;
	send_signal(SIGINT,fg_pid);
}

void handler_cntlz(int sig_num) {
	if(fg_pid == 0)
		return;
	send_signal(SIGTSTP,fg_pid);
}

