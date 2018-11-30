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
	printf("smash > signal %d was sent to pid %d\n",signal,pid);
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

void set_SIGINT(){
	struct sigaction cntlc_act;
	cntlc_act.sa_handler = &handler_cntlc;
	cntlc_act.sa_flags = SA_RESTART;
	sigfillset(&cntlc_act.sa_mask);

	sigaction(SIGINT, &cntlc_act, NULL);
}

void set_SIGTSTP(){
	struct sigaction cntlz_act;
	cntlz_act.sa_handler = &handler_cntlz;
	cntlz_act.sa_flags = SA_RESTART;
	sigfillset(&cntlz_act.sa_mask);

	sigaction(SIGTSTP, &cntlz_act, NULL);
}

