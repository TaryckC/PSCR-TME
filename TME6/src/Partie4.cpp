#include "rsleep.h"
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>

using namespace std;

// V1 fonction wait_till_pid :
/*
int wait_till(pid_t pid) {
	pid_t childPID;
	while (true) {
		if ((childPID = wait()) == -1) {
			return -1;
		}
		else if (childPID == pid) {
			return 0;
		}
	}
}
*/

volatile bool timeout = false;

void handler(int sig) {
	timeout = true;
}

// V2 fonction wait_till_pid :
int wait_till(pid_t pid, int sec) {
	signal(SIGALRM, handler);
	alarm(sec);
	pid_t childPID;
	while (true) {
		if (timeout) {
			return 0;
		}
		else if ((childPID = wait() == pid) {
			return pid;
		}
	}
}

int void main() {

}
