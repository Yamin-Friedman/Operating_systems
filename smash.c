/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include "commands.h"

#define MAX_LINE_SIZE 80


int fg_pid = 0; // This is a global variable because the signal handlers must be able to access it

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	/************************************/
	// Init globals
	char cmdString[MAX_LINE_SIZE];
	char lineSize[MAX_LINE_SIZE];
	char previous_dir[MAX_LINE_SIZE + 1];
	*previous_dir = '\0';
	job_node **jobs = malloc(sizeof(job_node*)); //This represents the list of jobs.
	*jobs = NULL;
	/************************************/
	set_SIGINT();
	set_SIGTSTP();
	/************************************/

	while (1)
	{
		printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize)-1]='\0';
		// perform a complicated Command
		if(!ExeComp(lineSize)) continue;
		// background command
		if(!BgCmd(lineSize, jobs, cmdString)) continue;
		// built in commands
		ExeCmd(jobs, lineSize, cmdString,previous_dir);

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
}

