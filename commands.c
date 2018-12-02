//		commands.c
//********************************************
// This module is in charge of handling all the commands run through the small shell.
//********************************************
#include "commands.h"
char history[50][MAX_LINE_SIZE];
int history_start = 0;
int history_end = 0;
int historyModuloFlag = 0;

//**************************************************************************************
// function name: quit_with_kill
// Description: closes down the shell by killing all background jobs in order and freeing the memory
// Parameters: job_node *jobs - A pointer to the jobs linked list
// Returns: exits the program successfully
//**************************************************************************************
void quit_with_kill(job_node **jobs){
	job_node *curr_node = *jobs;
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

	free(jobs);

	exit(0);
}

//**************************************************************************************
// function name: quit_without_kill
// Description: closes down the shell freeing the memory
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: exits the program successfully
//**************************************************************************************
void quit_without_kill(job_node **jobs){
	// needs to erase all dynamic memory
	job_node *curr_node = *jobs;
	job_node *next_node;

	while(curr_node != NULL){
		next_node = curr_node->next;
		free(curr_node);
		curr_node = next_node;
	}

	free(jobs);
	exit(0);
}


//**************************************************************************************
// function name: remove_job
// Description: Removes a job from the background jobs list based on the pid
// Parameters: int pid - the PID of the job to remove from the list
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: bool of success or failure
//**************************************************************************************
bool remove_job(int pid, job_node **jobs){
	job_node *curr_node = *jobs;
	job_node *next_node;
	if(curr_node != NULL && curr_node->pid == pid){
		*jobs = curr_node->next;
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

//**************************************************************************************
// function name: add_to_jobs
// Description: Adds a new job to the list of background jobs
// Parameters: int pID - PID of the job to add
// Parameters: char *cmdstring - program string of the job to add
// Parameters: bool stopped - If the program is running in the background or not
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void add_to_jobs(int pID, char *cmdstring, bool stopped, job_node **jobs){
	struct timespec curr_time;
	job_node *curr_node = *jobs;
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
		*jobs = new_job_node;
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

//**************************************************************************************
// function name: fg_command
// Description: Runs the background job based on job_num in the foreground
// Parameters: int job_num - if zero run the last program run in background otherwise run the job based on the number given
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void fg_command(int job_num, job_node **jobs) {
	int curr_job_num = 0;
	int status;
	job_node *curr_node = *jobs;

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
		remove_job(curr_node->pid,jobs);
	}
}

//**************************************************************************************
// function name: bg_command
// Description: Signals a job stopped in the background to continue running in the background
// Parameters: int job_num - if zero run the last program stopped in background otherwise run the job based on the number given
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void bg_command(int job_num, job_node **jobs){
	int curr_job_num = 0;
	job_node *curr_node = *jobs;
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

//**************************************************************************************
// function name: print_jobs
// Description: Prints out the list of jobs in the background and cleans up any jobs that have finished running
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void print_jobs(job_node **jobs){
	int job_num = 1;
	int status;
	bool first = TRUE;
	int res;
	struct timespec curr_time;
	if(clock_gettime(CLOCK_REALTIME,&curr_time) == -1){
		perror("gettime:");
		return;
	}
	job_node *curr_node = *jobs;
	job_node *next_node;
	job_node *prev_node = *jobs;
	while(curr_node != NULL){

		res = waitpid(curr_node->pid,&status,WNOHANG);

		if(res && (WIFEXITED(status) || WIFSIGNALED(status))){
			next_node = curr_node->next;
			if(first == TRUE){
				*jobs = next_node;
			}else{
				prev_node->next = next_node;
			}
			free(curr_node);
			curr_node = next_node;
			continue;
		}

		time_t time = curr_time.tv_sec - curr_node->start_time;
		if(curr_node->stopped)
			printf("[%d] %s : %d %d secs (Stopped)\n",job_num,curr_node->program,curr_node->pid,(int)time);
		else
			printf("[%d] %s : %d %d secs\n",job_num,curr_node->program,curr_node->pid,(int)time);
		job_num++;
		first = FALSE;
		prev_node = curr_node;
		curr_node = curr_node->next;
	}
}

//**************************************************************************************
// function name: kill_job
// Description: Sends a signal to a job running in background
// Parameters: int signal_num - Number of signal to send
// Parameters: int job_num - The number of the job in the job list
// Parameters: job_node **jobs - A pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void kill_job(int signal_num,int job_num, job_node **jobs){
	job_node *curr_node = *jobs;

	for(int i = 1; i < job_num; i++){
		if(curr_node == NULL)
			break;
		curr_node = curr_node->next;
	}

	if(curr_node == NULL){
		printf("smash error: > kill %d – job does not exist\n",job_num);
		return;
	}

	if(kill(curr_node->pid,signal_num) == -1){
		printf("smash error: > kill %d – cannot send signal\n",job_num);
	}else{
		printf("smash > signal %d was sent to pid %d\n",signal_num,curr_node->pid);
	}
}

//********************************************
// function name: ExeCmd
// Description: interprets and executes built-in commands
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Parameters: char* lineSize - Command string to be overwritten
// Parameters: char *cmdString - Command string
// Parameters:  char previous_dir[MAX_LINE_SIZE + 1] - pointer to previous directory
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(job_node **jobs, char* lineSize, char* cmdString, char previous_dir[MAX_LINE_SIZE + 1])
{
	char* cmd; 
	char* args[MAX_ARG];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
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
	if (history_start ==  49)// if array is full, go back to the start of array
	{
		history_start = 0;// move to start of array
		history_end = history_start + 1;
		historyModuloFlag = 1;
	}

	else
	{
		if (historyModuloFlag)// now end need to move
		{
			if (history_start == 48)
				history_end = 0;
			else history_end++;
		}

		history_start++;
	}
	//history[history_start] = cmd;
	strcpy(history[history_start], cmd );
	
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		if(num_arg != 1) {
			goto error;
		}

		char *curr_dir = malloc(MAX_LINE_SIZE + 1);
		if(curr_dir == NULL){
			printf("Malloc error\n");
			return 1;
		}
		curr_dir = getcwd(curr_dir,MAX_LINE_SIZE);
		if(curr_dir == NULL){
			perror("getcwd:");
			free(curr_dir);
			return 1;
		}

		if(*args[1] == '-'){
			if(previous_dir == NULL) {
				free(curr_dir);
				return 0;
			}

			if(chdir(previous_dir) == -1){
				perror("chdir:");
				free(curr_dir);
				return 1;
			}
		} else{
			if(chdir(args[1]) == -1){
				if(errno == ENOENT){
					printf("smash error: > “%s” - path not found\n",args[1]);
				}else {
					perror("chdir:");
				}
				free(curr_dir);
				return 1;
			}
		}

		strcpy(previous_dir,curr_dir);
		free(curr_dir);
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if(num_arg > 0){
			goto error;
		}

		char curr_dir[MAX_LINE_SIZE + 1];

		if(getcwd(curr_dir,MAX_LINE_SIZE) == NULL){
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

 		print_jobs(jobs);
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

			int i;
			for (i= history_start-1; i >=0 ; i--)
			{
				printf("%s\n", history[i]);
			}

			if (historyModuloFlag)
			{
				for (i = 49; i >= history_end; i--)
				{
					printf("%s\n", history[i]);
				}
			}

	}
		/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int job_num = 0;
		job_node *curr_node = *jobs;
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

		fg_command(job_num,jobs);

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

		bg_command(job_num,jobs);
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(num_arg != 2 || *args[1] != '-'){
			goto error;
		}

		int signal_num = atoi(args[1] + 1);
		if(signal_num == 0){
			goto error;
		}

		int job_num = atoi(args[2]);

		kill_job(signal_num,job_num,jobs);
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
		{
		if(num_arg != 2){
			goto error;
		}

		if(rename(args[1],args[2]) == -1){
			perror("rename:");
			return 1;
		} else{
			printf("%s has been renamed to %s\n",args[1],args[2]);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(num_arg > 1){
			goto error;
		}
		if(num_arg == 1 && strcmp(args[1],"kill") != 0)
			goto error;

   		if(num_arg == 1){
		    quit_with_kill(jobs);
	    } else{
		    quit_without_kill(jobs);
	    }
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString,FALSE,jobs);
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
// Parameters: char *args[MAX_ARG] - array of command arguments
// Parameters: char *cmdString - Command string
// Parameters: bool background - If the command should be run in background
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString,bool background, job_node **jobs)
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
				if(waitpid(pID,&status,WNOHANG) == 0)
					add_to_jobs(pID, cmdString,FALSE,jobs);
			}
			else {
				fg_pid = pID;
				pid_res = waitpid(pID,&status,WUNTRACED);
				if(pid_res == -1){
					perror("waitpid:");
					break;
				}
				if(WIFSTOPPED(status)) {
					add_to_jobs(pID, cmdString, TRUE,jobs);
				}
				fg_pid = 0;
			}
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: checks if it is a complicated command
// Parameters: char* lineSize - Command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		return 0;
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: char* lineSize - Command string to be overwritten
// Parameters: job_node **jobs - Pointer to the jobs linked list
// Parameters: char *cmdString - Command string
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, job_node **jobs, char *cmdString)
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

		ExeExternal(args, cmdString,TRUE,jobs);
		return 0;
		
	}
	return -1;
}

