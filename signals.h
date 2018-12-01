#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef enum { FALSE , TRUE } bool;


// The PID of the foreground process. is zero if no process is running in the foreground
extern int fg_pid;


//**************************************************************************************
// function name: send_signal
// Description: Sends a signal of number signal to the process based on pid and prints a message
// Parameters: int signal - The number of signal to send
// Parameters: int pid - The PID of the process to send the signal to
// Returns: void
//**************************************************************************************
void send_signal(int signal,int pid);

//**************************************************************************************
// function name: set_SIGINT
// Description: Sets the signal handler for SIGINT
// Parameters: none
// Returns: void
//**************************************************************************************
void set_SIGINT();

//**************************************************************************************
// function name: set_SIGTSTP
// Description: Sets the signal handler for SIGTSTP
// Parameters: none
// Returns: void
//**************************************************************************************
void set_SIGTSTP();

/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int);

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void handler_cntlz(int);

#endif

