#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main2 () {
	const int N = 3;
	int nbChildren1 = 0;
	int nbChildren2 = 0;

	std::cout << "main pid=" << getpid() << std::endl;
	pid_t newProcess;
	for (int i=1, j=N; i<=N && j==N && (newProcess = fork())==0 ; i++ ) {
		if (newProcess == 0) {
			nbChildren1 = 0;
		}
		else {
			++nbChildren1;
		}
		std::cout << " i:j " << i << ":" << j << std::endl;
		for (int k=1; k<=i && j==N ; k++) {
			if ( fork() == 0) {
				nbChildren2 = 0;
				j=0;
				std::cout << " k:j " << k << ":" << j << std::endl;
			}
			else {
				++nbChildren2;
			}
		}
	}
	//std::cout << nbChildren1+nbChildren2 << std::endl;
	for(int i=0; i<nbChildren1+nbChildren2; ++i) {
		wait(NULL);
	}

	return 0;
}
