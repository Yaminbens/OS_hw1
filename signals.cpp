// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

   #include "commands.h"
   #include "signals.h"

/*******************************************/
/* Name: zero_fg
   Synopsis: zero the members of foreground job */
/*******************************************/

void zero_fg(){
	fg_job.setPid(0);
	fg_job.setName("");
	fg_job.setTime(0);
}

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C / Control-Z */
/*******************************************/

void catch_int(int sig_num) {
	pid_t pid;
	int status;
	int fgInList = 0;
	switch (sig_num) {

		//CTRL C
		case SIGINT:
			pid = fg_job.getPid();
			for (vector<job>::iterator it=jobs.begin(); it != jobs.end(); ++it) {
				if (it->getPid() == pid) {
					jobs.erase(it);
					break;
				}
			}
			zero_fg();
			kill(pid, SIGINT);
			break;

		//CTRL Z
		case SIGTSTP:
			if (fg_job.getPid() == 0) {
				break;
			}
			kill(fg_job.getPid(), SIGTSTP);
			for (vector<job>::iterator it=jobs.begin(); it != jobs.end(); ++it) {
				if (it->getPid() == fg_job.getPid()){
					fgInList = 1;
					it->setStopTime();
					break;
				}
			}
			if (fgInList) {
				fgInList =0;
				zero_fg();
				break;
			}
			job curr_job;
			curr_job.setPid(fg_job.getPid());
			curr_job.setName(fg_job.getName());
			curr_job.setTime(0);
			curr_job.setStopTime();
			jobs.push_back(curr_job);
			zero_fg();

			break;

		case SIGCHLD:
			//cout << "catch SIGCHLD" << endl;
			pid = waitpid((pid_t)(-1), &status, WUNTRACED | WNOHANG);
			if (pid == 0 || WIFSTOPPED(status) || WIFCONTINUED(status)) {
				break;
			}
			//sleep(1);
			if (fg_job.getPid() == pid) {
				zero_fg();
			}
			for (vector<job>::iterator it=jobs.begin(); it != jobs.end(); ++it) {
				if (it->getPid() == pid) {
					jobs.erase(it);
					break;
				}
			}

			break;


		}

}
