#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct job_node {
	int pid;
	time_t start_time;
	char *program;
	bool stopped;
	struct job_node *next;

} job_node;

extern job_node *jobs;

void handler_cntlc(int);
void handler_cntlz(int);
void handler_sigchld(int);

#endif

