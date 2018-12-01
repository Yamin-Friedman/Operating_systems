// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"

//**************************************************************************************
// function name: send_signal
// Description: Sends a signal of number signal to the process based on pid and prints a message
// Parameters: int signal - The number of signal to send
// Parameters: int pid - The PID of the process to send the signal to
// Returns: void
//**************************************************************************************
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

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void handler_cntlz(int sig_num) {
	if(fg_pid == 0)
		return;
	send_signal(SIGTSTP,fg_pid);
}

//**************************************************************************************
// function name: set_SIGINT
// Description: Sets the signal handler for SIGINT
// Parameters: none
// Returns: void
//**************************************************************************************
void set_SIGINT(){
	struct sigaction cntlc_act;
	cntlc_act.sa_handler = &handler_cntlc;
	cntlc_act.sa_flags = SA_RESTART;
	sigfillset(&cntlc_act.sa_mask);

	sigaction(SIGINT, &cntlc_act, NULL);
}

//**************************************************************************************
// function name: set_SIGTSTP
// Description: Sets the signal handler for SIGTSTP
// Parameters: none
// Returns: void
//**************************************************************************************
void set_SIGTSTP(){
	struct sigaction cntlz_act;
	cntlz_act.sa_handler = &handler_cntlz;
	cntlz_act.sa_flags = SA_RESTART;
	sigfillset(&cntlz_act.sa_mask);

	sigaction(SIGTSTP, &cntlz_act, NULL);
}

