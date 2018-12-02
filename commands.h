#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "signals.h"


#define MAX_LINE_SIZE 80
#define MAX_ARG 20

typedef struct job_node {
	int pid;
	time_t start_time;
	char program[MAX_LINE_SIZE + 1];
	bool stopped;
	struct job_node *next;

} job_node;


extern char previous_dir[MAX_LINE_SIZE + 1];

//**************************************************************************************
// function name: ExeComp
// Description: checks if it is a complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs, char *cmdString);
int ExeCmd(job_node** jobs, char* lineSize, char* cmdString, char previous_dir[MAX_LINE_SIZE + 1]);

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: pointer to jobs, external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString,bool background, job_node **jobs);

#endif

