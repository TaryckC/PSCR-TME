#include "chat_common.h"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>

using namespace std;

// Structure d'un message :
/*
	Un message doit contenir en poremière position le PID du client l'ayant émit.
	Le PID dans le message est au maximum de longueur 10.
*/

//Sémaphores :
vector<pid_t> clients;

sem_t* semMax;
sem_t* semReceived;
sem_t* serverUsed;
sem_t* shmAccess;

int currentProcessConnected = 0;

// Fonction de nettoyage. Appelé à la fin de l'exécution du programme.
void cleanup() {
	// Le serveur doit prévenir les clients qu'il s'arrête :
	while(clients.size() != 0) {
		kill(clients[0], SIGINT);
		clients.erase(clients.begin());
	}

    sem_close(semReceived);
    sem_close(serverUsed);
    sem_close(semMax);
    sem_unlink("semReceived");
    sem_unlink("serverUsed");
    sem_unlink("semMax");
	sem_unlink("shmAccess");
    shm_unlink(strdup(to_string(getpid()).c_str()));
    cout << "Resources cleaned up." << endl;
    exit(EXIT_SUCCESS);
}

// Handler pour le signal SIGINT.
void handler(int sig) {
	cout << "Signal " << sig << " reçu. Nettoyage..." << endl;
	cleanup();
}

void fillMessageSpace(void* memory) {
    // Convertir la mémoire partagée en un tableau de messages
    message* messages = static_cast<message*>(memory);

    // Parcourir tous les messages et les initialiser
    for (int i = 0; i < MAX_MESS; ++i) {
        // Initialiser le type à FREE_MESSAGE_SPACE
        messages[i].type = FREE_MESSAGE_SPACE;

		// Initialiser le contenu à une chaîne vide
        memset(messages[i].content, '\0', sizeof(messages[i].content));

        // Ajouter un PID par défaut (-1) dans les 10 premiers caractères
        snprintf(messages[i].content, 10, "%-10d", 99999); // Formatage sur 10 caractères
    }

    std::cout << "System: Memory initialized with FREE_MESSAGE_SPACE." << std::endl;
}

// Récupère le PID dans le message reçu par le serveur.
pid_t getMsgPID(message* msg) {
    char pid_String[10];
    for (int i = 0; i < 10; ++i) {
        pid_String[i] = msg->content[i];
    }
    pid_String[9] = '\0'; // Terminaison de la chaîne

    return (pid_t)stoi(pid_String); // Conversion en entier
}

bool deconnection(message* msg) {
	pid_t pidClient = getMsgPID(msg);
	for (int i=0; i<clients.size(); ++i) {
		if (clients[i] == pidClient) {
			// Si le processus est trouvé alors il est retiré de la liste des connectés.
			clients.erase(clients.begin()+i);
			cout << "Proccess disconected." << endl;
			cout << --currentProcessConnected << "/" << MAX_USERS << endl;
			sem_post(semMax);
			return true;
		}
	}
	// Si un processus non connecté essaye de se déconnecter le programme se termine.
	cout << "The process has not been found." << endl;
	return false;
}

bool connection(message* msg) {
	sem_wait(semMax); // Décrément le nombre restant de place sur le serveur
	/*
		Le client faisant la demande de connection envoie un message avec pour type : 1
		Le serveur recoit ce message et dans le contenu doit pouvoir lire en première position le PID du client faisant la demande de connection.
		Si le PID n'est pas présent alors la demande de connection est ignorée -> Aucun message de cette utilisateur ne sera traité.
	*/
	pid_t client = getMsgPID(msg);
	for (pid_t c : clients) {
		if (c == client) {
			// Si le pid est déjà présent dans la liste des processus connecté la fonction renvoie false et ne l'ajoute pas à nouveau.
			cout << "Process is already connected." << endl;
			return false;
		}
	}
	// Si le pid n'est pas déjà dans la liste des processus connectés alors il est ajouté et la fonction renvoie true.
	clients.push_back(client);
	cout << "A new process has connected." << endl;
	cout << ++currentProcessConnected << "/" << MAX_USERS << endl;
	return true;
}

void readMessage(message* msg) {
    // Tableau pour stocker le contenu du message (90 est la taille maximale connue).
	pid_t sender = getMsgPID(msg);

    char content[90]; 
    int indice = 0;

    // Parcours du contenu jusqu'à la fin de la chaîne ou jusqu'à la taille maximale.
    for (int i = 10; i < 100 && msg->content[i] != '\0'; ++i) {
        content[indice++] = msg->content[i];
    }

    // Ajoute un caractère nul pour terminer la chaîne.
    content[indice] = '\0';

    // Afficher le contenu du message pour validation.
    cout << sender << " : " << content << endl;
}

message* getMessage(int pos, void* adr) {
	message* msg = ((message*) adr) + pos;
	cout << msg->content << endl;
	return msg;
}

void sendMessage(pid_t client, message* msg) {
	std::string clientName = strdup(to_string(client).c_str());

	int fd_client = shm_open(clientName.c_str(), O_RDWR, 0666);
    if (fd_client < 0) {
        perror("Erreur lors de shm_open (emit d'un client)");
        exit(EXIT_FAILURE);
    }

    void* clientMemory = mmap(0, sizeof(myshm), PROT_READ | PROT_WRITE, MAP_SHARED, fd_client, 0);
    if (clientMemory == MAP_FAILED) {
        perror("Erreur lors du mapping memoire (client)");
        close(fd_client);
        EXIT_FAILURE;
    }

	close(fd_client);

	message* messages = static_cast<message*>(clientMemory);
	for (int i=0; i<MAX_MESS; ++i) {
		if (messages[i].type == FREE_MESSAGE_SPACE) {
            strncpy(messages[i].content, msg->content, sizeof(messages[i].content) - 1);
            messages[i].content[sizeof(messages[i].content) - 1] = '\0'; // Assurer la terminaison
			messages[i].type = msg->type;
			break;
		}
	}

	sem_unlink(clientName.c_str());
}

int main() {
	// création d'un Handler pour un arrêt propre du programme :
	sigset_t setNeg;
	sigfillset(&setNeg);
	sigdelset(&setNeg, SIGINT);

	struct sigaction act;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = &handler;
	sigaction(SIGINT, &act, NULL);

	// Création du segment de mémoire partagé au lancement du programme :
	pid_t serveurID = getpid();
    string pidStr = to_string(serveurID); // Convertit pid en std::string
    char* pidChar = strdup(pidStr.c_str());

	int fd = shm_open(pidChar, O_CREAT | O_EXCL | O_RDWR, 066);
	if (fd < 0) {
		perror("Erreurlors de shm_open");
		exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, sizeof(myshm)) == -1) {
		perror ("Erreur lors de ftruncate");
		close(fd);
		exit(EXIT_FAILURE);
	}
	void* shm = mmap(0, sizeof (myshm), PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
	if (shm == MAP_FAILED) {
		perror("Erreur lors de mmap");
		close(fd);
		exit(EXIT_FAILURE);
	}

    fillMessageSpace(shm);

	close(fd);

	// Suppression des semaphores précédemment créer (facultatif ?)
	sem_unlink("semReceived"); //

	// Instanciation des semaphores :
	semReceived = sem_open("semReceived", O_CREAT, 0666, 0);
	if (semReceived == SEM_FAILED) {
		perror("Erreur lors de sem_open pour semReceived");
		exit(EXIT_FAILURE);
	}
	serverUsed = sem_open("serverUsed", O_CREAT, 0666, 1);
	if (serverUsed == SEM_FAILED) {
		perror("Erreur lors de sem_open pour serverUsed");
		exit(EXIT_FAILURE);
	}
	
	// Compte le nombre d'utilisateur connecté simultanément et bloque toute tentativ de connexion une fois la limite atteinte.
	semMax = sem_open("semMax", O_CREAT, 0666, MAX_USERS);
	if (semMax == SEM_FAILED) {
		perror("Erreur lors de sem_open pour semMax");
		exit(EXIT_FAILURE);
	}

	shmAccess = sem_open("shmAccess", O_CREAT, 0666, 1);
	if (shmAccess == SEM_FAILED) {
		perror("Erreur lors de sem_open pour shmAccess");
		exit(EXIT_FAILURE);
	}

	// Création Tableau de users ID :

	// TODO - Lecture message reçus des clients dans la sh
	// Valeur de type d'un message : -1 : Déconnexion - 0 : Message normale - 1 : connexion
	// -6 indique un emplacement libre (FREE_MESSAGE_SPACE)
	
	// numMsg sert de compteur circulaire pour identifier la position du message à lire.
	int numMsg = 0;
	
	// AFFICHAGE DU PID DU SERVEUR :
	cout << "SERVEUR'S PID : " << getpid() << endl;

while(true) {
    sem_wait(semReceived);
    sem_wait(shmAccess);
    message* messages = static_cast<message*>(shm);
    for (int i = 0; i < MAX_MESS; ++i) {
        message* currentMsg = &messages[i];
        long msgType = currentMsg->type;
        if (msgType == -1) {
            deconnection(currentMsg);
            currentMsg->type = FREE_MESSAGE_SPACE;
        }
        else if (msgType == 0) {
            readMessage(currentMsg);
            for (int j = 0; j < clients.size(); ++j) {
                sendMessage(clients[j], currentMsg);
            }
            currentMsg->type = FREE_MESSAGE_SPACE;
        }
        else if (msgType == 1) {
            connection(currentMsg);
            currentMsg->type = FREE_MESSAGE_SPACE;
        }
        // Si le type est FREE_MESSAGE_SPACE, on ne fait rien
    }
    sem_post(shmAccess);
}
}
