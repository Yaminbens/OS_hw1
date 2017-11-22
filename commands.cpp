//		commands.c
//********************************************
#include "commands.h"
#include <unistd.h>



//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd( char* lineSize, char* cmdString, char* last_pwd, list<string>& hist)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];

	//Check

	

	const char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
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
		getcwd(pwd, MAX_LINE_SIZE);
		if(num_arg == 1)
		{
			// cd to last dir
			if(!strcmp(args[1],"-"))
			{
				if(last_pwd)
				{
					if (chdir(last_pwd) == -1) {
						cout << "smash error: > " << args[1] <<  "(prev_path) - path not found" << endl;
					}
					cout << last_pwd << endl;
					strcpy(last_pwd,pwd);
				}else{
					cout << "smash error: > " << args[1] <<  "(prev_path) - path not found" << endl;
				}
			}
			// cd to new dir
			else{
				if(chdir(args[1]) == -1)
				{
					cout << "smash error: > " << args[1] <<  " - path not found" << endl;
				} else {
					strcpy(last_pwd,pwd);
				}
			}
		}else{
			illegal_cmd = TRUE;
		}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if(num_arg >0){
			illegal_cmd = TRUE;
		}else{
			getcwd(pwd, MAX_LINE_SIZE);
			printf("%s\n", pwd);
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if (num_arg!=2)
		{
			illegal_cmd = true;
		} else {
			//check file exists
			if(FILE * file = fopen(args[1], "r")){
				fclose(file);
				//try to rename file
				if(rename(args[1], args[2]) == 0){
					cout << args[1] << " has been renamed to " << args[2] << endl;
				} else {
					//couldnt change file name
					perror("problem with mv - couldnt change name of file");
				}
			} else {
				//couldnt find or access file
				perror("problem with mv - file might not exist or be immutable");
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if(num_arg >0){
			illegal_cmd = TRUE;
		}else{
			if(!hist.empty()){
				for(list<string>::iterator it=hist.begin(); it!=hist.end(); ++it)
				{
					std::cout << *it << '\n';
				}
			}
		}
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		if(num_arg >0){
					illegal_cmd = TRUE;
		}else{
			if(!jobs.empty()){
				for(unsigned int i = 0; i< jobs.size(); i++)
				{
					std::cout << "[" << i+1 << "] "
							<< jobs[i].getName() <<" : " <<  jobs[i].getPid()
							<< " " <<  jobs[i].getTime() << " "
							<< "secs" << '\n';
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if(num_arg >0){
					illegal_cmd = TRUE;
		}else{
			std::cout << "smash pid is " << getpid() << '\n';
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int currjob = -1;
		if(num_arg>1){
			illegal_cmd = TRUE;
		}else{
			// fg to certain job number
			if(num_arg == 1){
				if(atoi(args[1])>jobs.size() || atoi(args[1])<0){
					perror("invalid job index\n");
					return 1;
				}else{
					currjob =atoi(args[1])-1;
				}
			}else{
				// fg to last job
				time_t time_t = -1;
				for (vector<job>::iterator it =jobs.begin(); it != jobs.end(); ++it){
						if(it->getRunTime() > time_t){
							time_t = it->getRunTime();
							currjob = it - jobs.begin();
						}
				}
				// no jobs found
				if(currjob == -1){
					perror("no jobs found\n");
					return 1;
				}
			}

			cout << jobs[currjob].getName() << endl;
			//if job is stopped, we continue by sending signal
			if (jobs[currjob].isStopped()) {
				if (kill(jobs[currjob].getPid(), SIGCONT) == -1) {
					perror("signal failed\n");
					return 1;
				}
			}

			//update fg_job
			fg_job.setName(jobs[currjob].getName());
			fg_job.setPid(jobs[currjob].getPid());
			fg_job.setTime(jobs[currjob].getTime());
			fg_job.setStopped(jobs[currjob].isStopped());

			sleep(1);
			//wait for fg job to finish
			if (waitpid(fg_job.getPid(), NULL, WNOHANG) == 0) {
				waitpid(fg_job.getPid(), NULL, WUNTRACED);
			}

		}
	} 

	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		int lastStopped = -1;
		if(num_arg>1){
			illegal_cmd = TRUE;
		}else{
			// bg to certain job number
			if(num_arg == 1){
				if(atoi(args[1])>jobs.size() || atoi(args[1])<0){
					perror("invalid job index\n");
					return 1;
				}else{
					lastStopped =atoi(args[1]);
				}
			}else{
				//bg to last job that got ctrl-Z
				for (vector<job>::iterator it =jobs.begin(); it != jobs.end(); ++it){
					if(it->isStopped() && it->getStopTime() > 0){
						if(it->getStopTime() > lastStopped){
							lastStopped = it - jobs.begin();
						}
					}
				}
				// no jobs found
				if(lastStopped == -1){
					perror("no jobs found\n");
					return 1;
				}
			}
			//case job is stopped, send signal to continue it
			cout << jobs[lastStopped].getName() << endl;
			if (jobs[lastStopped].isStopped()) {
				if (kill(jobs[lastStopped].getPid(), SIGCONT) == -1) {
					perror("signal failed\n");
					return 1;
				}
				jobs[lastStopped].setStopTime();
				jobs[lastStopped].setStopped(true);
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		bool complete = false;
		if(num_arg != 2){
			illegal_cmd = TRUE;
		}
		if(string(args[1]).substr(0,1) != "-"){
			illegal_cmd = TRUE;
		}
		int sig = -atoi(string(args[1]).c_str());
		int jobt = atoi(args[2]);
		if(jobs.size()<jobt){
			cout << "smash error: > kill " << jobt << " - job does not exist" << '\n';
			complete = true;
		}
		//send signal to job
		if(!illegal_cmd && !complete){
			if(kill(jobs[jobt-1].getPid(),sig) == -1){
				cout << "smash error: > kill " << jobt << " - cannot send signal" << '\n';
			}
		}
	}/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		//simple quit
		if (num_arg == 0) {
			exit(1);
		}
		//quit kill
		if (num_arg==1 && strcmp(args[1],"kill") == 0){
			for (vector<job>::iterator it =jobs.begin(); it != jobs.end(); ++it){
				bool invalidFlag = false;
				if (it->getPid()>0) {
					cout << "[" << it-jobs.begin() << "] " << it->getName() << " - Sending  ... ";
					if (kill(it->getPid(), SIGTERM)){
						invalidFlag = true;
						it->setPid(0);
						it->setName("");
						it->setRunTime();
						it->setStopped(TRUE);
						it->setStopTime();
						cout << "Done." << endl;
						break;
					}else{
						//waits 5 seconds for job to terminate
						sleep(5);
					}
					// job has not terminated in 5 seconds so we force termination
					if (invalidFlag==false)
					{
						cout << "(5 sec passed) ";
						cout << "Sending SIGKILL... ";
						if (!kill(it->getPid(), SIGKILL))
							perror("problem with killing jobs");
						else
							cout << "Done." << endl;
					}
				}
			}
			exit(1);
		}
	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmd);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{

		int pID = fork();
		switch (pID)
		{
			case -1:
					perror("Failed to execute");
					break;

			case 0 :
					// Child Process
					// execute command and kill process
					setpgrp();
					execvp(cmdString, args);
					perror("invalid command\n");
					exit(1);
					break;

			default:
					//update current fg job
					fg_job.setName(cmdString);
					fg_job.setPid(pID);
					fg_job.setTime(time(0)); ///MAYA: right?
					fg_job.setStopped(FALSE);

					if (waitpid(pID, NULL, WNOHANG) == 0) {
						//wait for fg job to finish
						waitpid(pID, NULL, WUNTRACED);
					}
					break;
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
	//char ExtCmd[MAX_LINE_SIZE+2];
	char *arguments[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	cout << "got in Execomp" << endl;
    	char st[10] = "/bin/sh";

		arguments[0] = st;
		arguments[1] = "-c";
		arguments[2] = "-f";
		arguments[3] = lineSize;
		arguments[4] = NULL;

		ExeExternal(arguments, st);

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
int BgCmd(char* lineSize)
{
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		int num_arg = 0;
			// set command and arguments
			Command = strtok(lineSize, delimiters);
			if (!Command)
				return 0;
			args[0] = Command;
			for (int i = 1; i < MAX_ARG; i++) {
				args[i] = strtok(NULL, delimiters);
				num_arg+=1;
				if (args[i] == NULL){
					num_arg-=1;
				}
			}
			int pID = fork();
			switch(pID)
			{
				case -1:
					perror("Failed to execute");
					break;
				case 0:
					// Child Process
					// execute command and kill process
					setpgrp();
					execvp(Command, args);
					perror("invalid command\n");
					exit(1);
					break;

				default:
					//update jobs list
					job new_job;
					new_job.setName(Command);
					new_job.setPid(pID);
					new_job.setTime(time(0));
					new_job.setStopped(false);
					jobs.push_back(new_job);
					return 0;
					break;
			}
		}
	return -1;
}

