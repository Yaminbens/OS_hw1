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

#define MAXARGS 20

//CPP
#include <iostream>
using namespace std;
#include <list>
#include <string>
#include <sstream>
#include <vector>



class job
{
  protected:
	//char* name_;
	char name_[MAXARGS];
	long pid_, time_created_;
	bool stopped_;
	time_t runTime_;
	time_t stopTime_;
  public:

	char* getName() {return name_;};
	time_t getTime() {return time(0)-time_created_;};
	void setTime(time_t ttime) {time_created_ = ttime;};
	void setPid(int pid) { pid_ = pid;};
	int getPid() {return pid_;};
	void setName(const char* name) {strcpy(name_,name);};
	bool isStopped() {return stopped_;};
	void setStopped(bool stoped) {stopped_= stoped;};
	long getRunTime() {return runTime_;};
	void setRunTime() {runTime_=time(0);};
	time_t getStopTime() {return stopTime_;};
	void setStopTime() {stopTime_=time(0);};
};

extern vector<job> jobs;
extern job fg_job;

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef enum {FALSE , TRUE } Bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize);
int ExeCmd( char* lineSize, char* cmdString, char* last_pwd, list<string>& hist);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
#endif

