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

//CPP
#include <iostream>
using namespace std;
#include <list>
#include <string.h>


#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef enum {FALSE , TRUE } Bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(void* jobs, char* lineSize, char* cmdString, char* last_pwd, list<string>& hist);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
#endif

