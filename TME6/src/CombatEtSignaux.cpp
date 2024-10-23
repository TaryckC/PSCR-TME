#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "rsleep.h"
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

pid_t fils;

int pointDeVie = 3;

void handler(int sig) {
	cout << ((fils==0)? "fils" : "père") << " : Ouch ! Il me reste : " << --pointDeVie << " pdv." << endl;
	if (pointDeVie == 0) {
		cout << "Le " << ((fils==0)? "fils" : "père") << "... est mort." <<  endl;
		_exit(1);
	}

}

void attaque(pid_t adversaire) {
	//Création du mask
	sigset_t setNeg;
	sigfillset(&setNeg);
	sigdelset(&setNeg, SIGINT);

	struct sigaction act;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = handler;
	sigaction(SIGINT, &act, NULL);

	if (kill(adversaire, SIGINT) == -1) {
		exit(0);
	}
	cout << ((fils==0)? "fils" : "père") << " : Pew !" << endl;
	randsleep();
}

void defense() {
	sigset_t setNeg;
	sigfillset(&setNeg);
	sigdelset(&setNeg, SIGINT);

	struct sigaction act;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = SIG_IGN;
	sigaction(SIGINT, &act, NULL);
	randsleep();
}

void combat(pid_t adversaire) {
	while(true) {
		defense();
		attaque(adversaire);
		if (fils != 0) {
			waitpid(0, NULL, WNOHANG);
			//Nous permet de collecter le procesus
			// Evite de continuer à boucler parce que le processus fils est zombie.
			// --> le kill renverra bien -1 car le processus n'est plus.
		}
	}
}

int main() {
	fils = fork();
	if (fils == 0) {
		combat(getppid());
	}
	else {
		combat(fils);
	}
}
