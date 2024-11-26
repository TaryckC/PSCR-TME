#include <iostream>
#include <unistd.h>

//SLIDE 58-59 cours 7 similaire à ce qui est demandé

using namespace std;

int main(int argc, char **argv) {
	if (argc==0) {
		cout << "Pipe requires arguments" << endl;
		return -1;
	}

	int tube[2];
	pipe(tube);
	pid_t fils;
	bool pipeFound = false;
	char* args1[argc];
	char* args2[argc];
	int ind1 = 0; //Nombre d'arguments
	int ind2 = 0; //Le premier élément est réservé au résultat de la commande avant pipe

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "|") == 0) {
            pipeFound = true;
        } else if (pipeFound) {
            args2[ind2++] = argv[i];
        } else {
            args1[ind1++] = argv[i];
        }
    }

    args1[ind1] = nullptr;
    args2[ind2] = nullptr;

	if (pipe(tube) == -1) {
		perror("pipe");
		exit(1);
	}

	if((fils = fork()) == -1) {
		perror("fork");
		exit(2);
	}


	if(fils == 0) {
		dup2(tube[1], STDOUT_FILENO);
		close(tube[1]);
		close(tube[0]);

		if(execvp(args2[0], args1) == -1) {
			perror("execvp failed");
			exit(3);
		}

	}

	else {
		dup2(tube[0], STDIN_FILENO);
		close(tube[1]);
		close(tube[0]);

		if(execvp(args1[0], args2) == -1) {
			perror("execvp failed");
			exit(4);
		}
	}


}
