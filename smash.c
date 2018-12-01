/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include "commands.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20


char* L_Fg_Cmd;
int fg_pid = 0;

char lineSize[MAX_LINE_SIZE];
//char history[50][MAX_LINE_SIZE];
//char *history_start_ptr = history;
//char *hisory_end_ptr = history;
char previous_dir[MAX_LINE_SIZE + 1];
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	set_SIGINT();
	set_SIGTSTP();
	/************************************/

	/************************************/
	// Init globals 
	*previous_dir = '\0';
	job_node *jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)

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
		ExeCmd(jobs, lineSize, cmdString);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
}

