// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/

#include "signals.h"

bool remove_job(int pid){
	job_node *curr_node = jobs;
	job_node *next_node;
	if(curr_node != NULL && curr_node->pid == pid){
		jobs = curr_node->next;
		free(curr_node);
		return true;
	}
	while(curr_node != NULL){
		next_node = curr_node->next;
		if(next_node != NULL && next_node->pid == pid){
			curr_node->next = next_node->next;
			free(next_node);
			return true;
		}
		curr_node = curr_node->next;
	}
	return false;
}
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num) {
	if(kill(fg_pid,SIGINT) == -1){
		perror("kill SIGINT:");
		return;
	}
	printf("signal SIGINT was sent to pid %d\n",fg_pid);
}

void handler_cntlz(int sig_num) {
	if(kill(fg_pid,SIGTSTP) == -1){
		perror("kill SIGTSTP:");
		return;
	}
	printf("signal SIGTSTP was sent to pid %d\n",fg_pid);
}

void handler_sigchld(int sig_num){
	int child_id;
	job_node *curr_node = jobs;
	while (curr_node != NULL) {
		child_id = waitpid(curr_node->pid,NULL,WNOHANG);
		if (child_id == -1) {
			perror("waitpid:");
		}

		if(child_id == fg_pid){
			return;;
		}

		curr_node = curr_node->next;
		if(child_id > 0) {
			if (!remove_job(child_id)) {
				printf("Failed to remove child process\n");
			}
		}
	}
}

