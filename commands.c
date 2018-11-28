//		commands.c
//********************************************
#include "commands.h"

void quit_with_kill(){
	job_node *curr_node = jobs;
	job_node *next_node = NULL;
	int job_num = 0;
	int status;
	int res;

	while(curr_node != NULL){
		job_num++;
		printf("[%d] %s %d - Sending SIGTERM... ",job_num,curr_node->program,curr_node->pid);
		res = kill(curr_node->pid,SIGTERM);
		if(res == -1){
			perror("kill:");
		}

		res = waitpid(curr_node->pid,&status,WUNTRACED|WNOHANG);
		if(res == -1){
			perror("waitpid:");
			break;
		}

		if(WIFSIGNALED(status)){
			printf("Done.\n");
		}
		else{

			sleep(5);

			res = waitpid(curr_node->pid,&status,WUNTRACED|WNOHANG);
			if(res == -1){
				perror("waitpid:");
				break;
			}

			if(WIFSIGNALED(status)){
				printf("Done.\n");
			}
			else {
				printf(" (5 sec passed) Sending SIGKILL... Done.\n");
				res = kill(curr_node->pid, SIGKILL);
				if (res == -1) {
					perror("kill:");
				}
			}
		}

		next_node = curr_node->next;
		free(curr_node);
		curr_node = next_node;
	}

	exit(0);
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

bool remove_job(int pid){
	job_node *curr_node = jobs;
	job_node *next_node;
	if(curr_node != NULL && curr_node->pid == pid){
		jobs = curr_node->next;
		free(curr_node);
		return TRUE;
	}
	while(curr_node != NULL){
		next_node = curr_node->next;
		if(next_node != NULL && next_node->pid == pid){
			curr_node->next = next_node->next;
			free(next_node);
			return TRUE;
		}
		curr_node = curr_node->next;
	}
	return FALSE;
}

void add_to_jobs(int pID, char *cmdstring, bool stopped){
	struct timespec curr_time;
	job_node *curr_node = jobs;
	job_node *new_job_node = (job_node*)malloc(sizeof(job_node));
	if(new_job_node == NULL){
		printf("malloc error\n");
		exit(-1);
	}

	new_job_node->stopped = stopped;
	new_job_node->pid = pID;
	new_job_node->next = NULL;
	strcpy(new_job_node->program,cmdstring);

	while(curr_node != NULL){
		if(curr_node->next == NULL){
			break;
		}
		curr_node = curr_node->next;
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

void fg_command(int job_num) {
	int curr_job_num = 0;
	int status;
	job_node *curr_node = jobs;

	while(curr_node->next != NULL){
		curr_job_num++;
		if(job_num && curr_job_num  == job_num){
			break;
		}

		curr_node = curr_node->next;
	}

	if(curr_node->stopped){
		curr_node->stopped = FALSE;
		send_signal(SIGCONT,curr_node->pid);
	}

	fg_pid = curr_node->pid;
	waitpid(curr_node->pid,&status,WUNTRACED);
	fg_pid = 0;
	if(WIFSTOPPED(status)) {
		curr_node->stopped = TRUE;
	}
	if(WIFSIGNALED(status)){
		remove_job(curr_node->pid);
	}
}

void bg_command(int job_num){
	int curr_job_num = 0;
	job_node *curr_node = jobs;
	job_node *last_stopped = NULL;
	if(curr_node == NULL){
		return;
	}

	while(curr_node != NULL){
		curr_job_num++;
		if(curr_node->stopped){
			last_stopped = curr_node;
		}
		if(job_num && curr_job_num  == job_num){
			break;
		}

		if(curr_node->next == NULL)
			break;

		curr_node = curr_node->next;
	}

	if(!job_num){
		curr_node = last_stopped;
		if(last_stopped == NULL){
			return;
		}
	}
	else{
		if(!curr_node->stopped){
			return;
		}
	}

	curr_node->stopped = FALSE;
	printf("%s\n",curr_node->program);
	send_signal(SIGCONT,curr_node->pid);
}

void print_jobs(){
	int job_num = 1;
	struct timespec curr_time;
	if(clock_gettime(CLOCK_REALTIME,&curr_time) == -1){
		perror("gettime:");
		return;
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
		if(num_arg > 0){
			goto error;
		}

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
		if(num_arg > 0){
			goto error;
		}

 		print_jobs();
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if(num_arg > 0){
			goto error;
		}

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
		job_node *curr_node = jobs;
		if(curr_node == NULL){
			return 0;
		}

		if(num_arg > 1){
			goto error;
		}
		else if( num_arg == 1) {
			job_num = strtol(args[1],NULL,10);
			if(!job_num){
				goto error;
			}
		}

		fg_command(job_num);

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		int job_num = 0;

		if(num_arg > 1){
			goto error;
		}
		else if( num_arg == 1) {
			job_num = strtol(args[1],NULL,10);
			if(!job_num){
				goto error;
			}
		}

		bg_command(job_num);
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(num_arg > 1){
			goto error;
		}
		if(num_arg == 1 && !strcmp(args[1],"kill"))
			goto error;

   		if(num_arg == 1 && strcmp(args[1],"kill")){
		    quit_with_kill();
	    } else{
		    quit_without_kill();
	    }
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString,FALSE);
	 	return 0;
	}
    return 0;

	error:
	printf("smash error: > \"%s\"\n", cmdString);
	return 1;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString,bool background)
{
	int pID;
	int pid_res = 0;
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
				add_to_jobs(pID, cmdString,FALSE);
			}
			else {
				fg_pid = pID;
				pid_res = waitpid(pID,&status,WUNTRACED);
				if(pid_res == -1){
					perror("waitpid:");
					break;
				}
				if(WIFSTOPPED(status)) {
					add_to_jobs(pID, cmdString, TRUE);
				}
				if(WIFSIGNALED(status)){
				}
				fg_pid = 0;
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
		return 0;
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

		ExeExternal(args, cmdString,TRUE);
		return 0;
		
	}
	return -1;
}

