// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

   #include "commands.h"
   #include "signals.h"

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C / Control-Z */
/*******************************************/


void catch_int(int sig_num) {
	pid_t pid;
	int status;
	int fgInList = 0;
	switch (sig_num) {

	case SIGTSTP: //CTRL+Z was pressed
		if (fg_job.getPid() == 0) {
			break;
		}
		//printf("handling SIGTSTP, pid is %d\n", fg_job.getPid());
		kill(fg_job.getPid(), SIGTSTP); //send sigstp to fp process
		for (vector<job>::iterator it=jobs.begin(); it != jobs.end(); ++it) {//look for job in jobs list
			if (it->getPid() == fg_job.getPid()){
				fgInList = 1;
				it->setStopTime();
				break; //if exists in jobs list
			}
		}
		if (fgInList) {
			fgInList =0;
			fg_job.setPid(0); // reset fp_job
			fg_job.setName("");
			fg_job.setTime(0);
			break;
		}
		for (unsigned int i = 0; i < jobs.size(); i++) { //find place in jobs list and add process
			if (jobs[i].getPid() == 0) {
				jobs[i].setPid(fg_job.getPid());
				jobs[i].setName(fg_job.getName());
				jobs[i].setTime(0);
				jobs[i].setStopTime();
				 // reset fp_job

				fg_job.setPid(0);
				fg_job.setName("");
				fg_job.setTime(0);
				//printf("done\n");
				break;
			}
		}
		//printf("done\n");
		break;

	case SIGCHLD:
		//printf("SIGCHILD\n");
		pid = waitpid((pid_t)(-1), &status, WUNTRACED | WNOHANG); //wait for status change or feedback
		//printf("handling SIGCHILD, pid is %d\n", pid);
		if (pid == 0) { //if pid handled is 0 move on
			//printf("MOVING ON ITS PID 0!!!");
			break;
		}
		//if sigstop or sigint (or sogcont) do nothing.
		if (WIFSTOPPED(status)) { // if stopped by stsp move on
			//printf("STOPPED BY SIGNAL, breaking.\n");
			break;
		}
		if (WIFCONTINUED(status)) { // if continued by sigcont move on
			//printf("child continued, breaking.\n");
			break;
		}

		// process apparently finished or killed.
		if (fg_job.getPid() == pid) { // if it was the job in fg, reset fg
			// delete fg_job
			//printf("fg_job! child finished\n");
			fg_job.setPid(0);
			fg_job.setName("");
			fg_job.setTime(0);
		}
		// look for process in jobs and remove from jobs list
		//printf("looking for job to delete\n");
		for (unsigned int i = 0; i < jobs.size(); i++) {
			if (jobs[i].getPid() == 0) {
				jobs[i].setPid(0);
				jobs[i].setName("");
				jobs[i].setTime(0);
				jobs[i].setStopTime();
				break;
			}
		}
		break;

	case SIGINT: //CTRL+C was pressed
		//printf("handling SIGINT, pid is %d\n", fg_job.getPid());
		pid = fg_job.getPid();
		for (vector<job>::iterator it=jobs.begin(); it != jobs.end(); ++it) { // find place and add to jobs list
			if (it->getPid() == pid) {
				jobs.erase(it);
				break;
			}
		}
		fg_job.setPid(0); //reset fp job
		fg_job.setName("");
		fg_job.setTime(0);
		kill(pid, SIGINT); // send sigint to process
		//printf("done\n");
		break;
	}

}
