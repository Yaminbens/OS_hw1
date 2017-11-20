/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80

#define MAX_HISTORY 50



//CPP
#include <iostream>
using namespace std;
#include <list>
#include <vector>
#include <string>
#include <ctime>

char* L_Fg_Cmd;
struct sigaction act;

vector<job> jobs;
job fg_job;



char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: history_update
// Description:
//**************************************************************************************
void history_update(list<string>&  hist, char* cmd)
{
	if(hist.size() > 50)
	{
		hist.pop_front();
	}
	char command[MAX_LINE_SIZE];
	strcpy(command, cmd);
	hist.push_back(command);
}

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
    act.sa_handler = &catch_int;
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */

	/************************************/
    sigaction(SIGTSTP, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGINT, &act, NULL);
	/************************************/
	// Init globals 
    char last_pwd[MAX_LINE_SIZE];
    list<string> hist;

	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);


		//
		cmdString[strlen(lineSize)-1]='\0';
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue;
					// background command	
	 	if(!BgCmd(lineSize)) continue;
					// built in commands
	 	ExeCmd(lineSize,cmdString,last_pwd,hist);
		
		//changes - yamin
		history_update(hist,cmdString);
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
    	}
    return 0;
}

