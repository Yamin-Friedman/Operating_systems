//		commands.c
//********************************************
#include "commands.h"

void quit_with_kill(){

}

void quit_without_kill(){
	// needs to erase all dynamic memory
	job_node *curr_node = jobs;
	job_node *next_node;
	while(curr_node != NULL){
		next_node = curr_node->next;
		free(curr_node);
		curr_node = next_node;
	}
	exit(0);
}

bool check_valid_num(char *string,int *num){
	*num = 0;
	while (string != "\0"){
		*num *= 10;
		if(*string >= '0' && *string <= '9'){
			*num += *string - 48;
		}
		else{
			return FALSE;
		}
		string++;
	}
	return TRUE;
}

void add_to_jobs(int pID, char *cmdstring){
	struct timespec curr_time;
	job_node *curr_node = jobs;
	job_node *new_job_node = (job_node*)malloc(sizeof(job_node));
	if(new_job_node == NULL){
		printf("malloc error\n");
		exit(-1);
	}

	new_job_node->stopped = TRUE;
	new_job_node->pid = pID;
	new_job_node->next = NULL;
	strcpy(new_job_node->program,cmdstring);

	while(curr_node != NULL){
		if(curr_node->next == NULL){
			break;
		}
	}
	if(curr_node == NULL){
		jobs = new_job_node;
	}
	else {
		curr_node->next = new_job_node;
	}
	if(clock_gettime(CLOCK_REALTIME,&curr_time) == -1){
		perror("gettime:");
		return;
	}
	new_job_node->start_time = curr_time.tv_sec;
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(job_node* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		if (num_arg == 1)
		{
			if (args[1] == NULL)
			{
				printf("smash error:> path not found\n");
				return 1;
			}
			char *currLocation;
			currLocation = getcwd(currLocation, sizeof(*currLocation));
			if (args[1] == '-')
			{// move to last location
				int lastLocationflag = chdir(lastLocation);
				if (lastLocationflag)
				{
					printf("smash error:> path not found\n");
					return 1;
				}
				strcpy(lastLocation, currLocation);
				return 0;
			}
			else {
				//move to new location
				int anotherLocation = chdir(args[1]);
				if (anotherLocation != 0) {
					printf("smash error:> %s path not found\n", args[1]);
					return 1;
				}
				strcpy(lastLocation,currLocation);
				return 0;
			}


		}
		else {
			illegal_cmd = TRUE;
		}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		char curr_dir[MAX_LINE_SIZE + 1];

		if(getcwd(curr_dir,MAX_LINE_SIZE) == -1){
			perror("getcwd:");
			return -1;
		}

		printf("%s\n",curr_dir);
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		int job_num = 1;
		struct timespec curr_time;
		if(clock_gettime(CLOCK_REALTIME,&curr_time) == -1){
			perror("gettime:");
			return -1;
		}
		job_node *curr_node = jobs;

		while(curr_node != NULL){
			time_t time = curr_time.tv_sec - curr_node->start_time;
			if(curr_node->stopped)
				printf("[%d] %s : %d %d secs (Stopped)\n",job_num,curr_node->program,curr_node->pid,(int)time);
			else
				printf("[%d] %s : %d %d secs\n",job_num,curr_node->program,curr_node->pid,(int)time);
			job_num++;
			curr_node = curr_node->next;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		int pid = getpid();
		printf("smash pid is %d\n",pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{

	}
		/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int job_num = 0;
		int curr_job_num = 0;
		int status;
		job_node *curr_node = jobs;
		if(curr_node == NULL){
			return 0;
		}

		if(num_arg > 1){
			goto error;
		}
		else if( num_arg == 1) {
			if (!check_valid_num(args[1], &job_num)) {
				goto error;
			}
		}
		while(curr_node->next != NULL){
			curr_job_num++;
			if(job_num && curr_job_num  == job_num){
				break;
			}

			curr_node = curr_node->next;
		}

		if(curr_node->stopped){
			curr_node->stopped = FALSE;
			if(kill(curr_node->pid,SIGCONT) == -1){
				perror("kill:");
				return 1;
			}
			printf("smash > signal SIGCONT was sent to pid %d\n",curr_node->pid);
		}

		fg_pid = curr_node->pid;
		waitpid(curr_node->pid,&status,WUNTRACED);

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		int job_num = 0;
		int curr_job_num = 0;
		job_node *curr_node = jobs;
		job_node *last_stopped = NULL;
		if(curr_node == NULL){
			return 0;
		}

		if(num_arg > 1){
			goto error;
		}
		else if( num_arg == 1) {
			if (!check_valid_num(args[1], &job_num)) {
				goto error;
			}
		}
		while(curr_node->next != NULL){
			curr_job_num++;
			if(curr_node->stopped){
				last_stopped = curr_node;
			}
			if(job_num && curr_job_num  == job_num){
				break;
			}

			curr_node = curr_node->next;
		}

		if(!job_num){
			curr_node = last_stopped;
			if(last_stopped == NULL){
				return 0;
			}
		}
		else{
			if(!curr_node->stopped){
				return 0;
			}
		}

		curr_node->stopped = FALSE;
		printf("%s\n",curr_node->program);
		if(kill(curr_node->pid,SIGCONT) == -1){
			perror("kill:");
			return 1;
		}
		printf("smash > signal SIGCONT was sent to pid %d\n",curr_node->pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		if(num_arg == 1 && strcmp(args[1],"kill")){
		    quit_with_kill();
	    } else{
		    quit_without_kill();
	    }
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, num_arg, cmdString,FALSE);
	 	return 0;
	}
    return 0;

	error:
	printf("smash error: > \"%s\n", cmdString);
	return 1;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG],int num_args, char* cmdString,bool background)
{
	int pID;
	int status;
	switch(pID = fork())
	{
		case -1:
			perror("fork:");
			break;
		case 0 :
                	// Child Process
			setpgrp();
			status = execvp(args[0],args);
			if(status == -1){
				perror("execv:");
				exit(-1);
			}
		default:
			if(background){
				add_to_jobs(pID, cmdString);
			}
			else {
				fg_pid = pID;
				waitpid(pID, &status, WUNTRACED);
				if (WIFSTOPPED(status)) {
					add_to_jobs(pID, cmdString);
				}
			}
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs, char *cmdString)
{

	char* cmd;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	int i = 0, num_arg = 0;
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		cmd = strtok(lineSize, delimiters);
		if (cmd == NULL)
			return 0;
		args[0] = cmd;
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL)
				num_arg++;

		}

		ExeExternal(args, num_arg, cmdString,TRUE);
		return 0;
		
	}
	return -1;
}

