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

//**************************************************************************************
// function name: ExeComp
// Description: checks if it is a complicated command
// Parameters: char* lineSize - Command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize);

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: char* lineSize - Command string to be overwritten
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Parameters: char *cmdString - Command string
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, job_node **jobs, char *cmdString);

//********************************************
// function name: ExeCmd
// Description: interprets and executes built-in commands
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Parameters: char* lineSize - Command string to be overwritten
// Parameters: char *cmdString - Command string
// Parameters:  char previous_dir[MAX_LINE_SIZE + 1] - pointer to previous directory
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(job_node** jobs, char* lineSize, char* cmdString, char previous_dir[MAX_LINE_SIZE + 1]);

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: char *args[MAX_ARG] - array of command arguments
// Parameters: char *cmdString - Command string
// Parameters: bool background - If the command should be run in background
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool background, job_node **jobs);

#endif

