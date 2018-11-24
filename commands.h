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
#include "signals.h"

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

extern int fg_pid;
extern char history[50][MAX_LINE_SIZE];
extern char *history_start_ptr = history;
extern char *hisory_end_ptr = history;
typedef struct job_node job_node;
typedef enum { FALSE , TRUE } bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(job_node* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG],int num_args, char* cmdString);
#endif

