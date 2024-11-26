#include "chat_common.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <atomic>

using namespace std;

// mémoire serveur :
void* sharedMemory;

// flag d'arrêt d'exécution pour les threads :
std::atomic<bool> stop = false;

// Sémaphore pour accès concurent : mémoire message reçus
sem_t* semReceived;
sem_t* shmAcces;
sem_t* semServerReceivedMsg;

// vecteur pour gérer les threads :
vector<thread> threads;

// type du message : FREE_MESSAGE_SPACE (espace de mémoire libre) | -1 (déconnexion) | 0 message | 1 connexion
void writeUserMessage(char* content, void* memory, long type) {
    // On convertit la mémoire partagé en un tableau de message :
    message* messages = static_cast<message*>(memory);

    for (int i=0; i<MAX_MESS; ++i) {
        // Si l'emplacement est libre
        if (messages[i].type == FREE_MESSAGE_SPACE) {
            // Préparer le message
            strncpy(messages[i].content, content, sizeof(messages[i].content) - 1);
            messages[i].content[sizeof(messages[i].content) - 1] = '\0'; // Assurer la terminaison
            messages[i].type = type;
            sem_post(semServerReceivedMsg);
            return;
        }
    }
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

char* GetUserMessage(char* content) {
    int length = 0; // Initialiser un compteur pour la taille
    char* message = new char[100]; // Taille maximale du message (10 pour le PID + 80 pour le contenu)
    //char content[90]; // Contient le contenu du message envoyé

    // Ajout du PID du processus au début du message
    pid_t pid = getpid(); // Récupérer le PID du processus
    snprintf(message, 11, "%-10d", pid); // Insère le PID, aligné sur 10 caractères

    // Parcourir la chaîne jusqu'à rencontrer le caractère nul '\0'
    for (char* ptr = content; *ptr != '\0'; ++ptr) {
        if (length >= 90) { // Limite à 80 caractères pour le contenu
            cout << "System : Message is too long, only part of it will be sent." << endl;
            break;
        }
        message[10 + length] = *ptr; // Écrit à partir de l'index 10
        //content[length] = *ptr;
        ++length;
    }

    // Terminer la chaîne avec un caractère nul
    message[10 + length] = '\0';

    return message;
}

void disconnect(void* memory) {
    cout << "System : " << "Sending disconnection message ..." << endl;
    writeUserMessage(strdup(to_string(getpid()).c_str()), memory, -1);
    cout << "System : " << " You are disconnected." << endl;
}

// Fonction de netoyage :
void cleanup() {
    disconnect(sharedMemory);
    sem_close(semReceived);
    sem_close(shmAcces);
    if (sem_unlink(("semReceived" + std::to_string(getpid())).c_str()) == -1) {
        perror("Erreur lors de sem_unlink");
    }

    stop = true;
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }    
}

// Handler fin de programme :
void handler(int sig) {
    cout << "Signal " << sig << " reçu. Le client s'arrête...";
    cleanup();
}

// Méthodes en double : Existe aussi dans chat_server

// Récupère le PID dans le message reçu par le serveur.
pid_t getMsgPID(message* msg) {
    char pid_String[10];
    for (int i = 0; i < 10; ++i) {
        pid_String[i] = msg->content[i];
    }
    pid_String[9] = '\0'; // Terminaison de la chaîne

    return (pid_t)stoi(pid_String); // Conversion en entier
}

// À quelques choses prêt, cette méthode renvoie le char* tandis que celle du serveur l'affiche seulement
char* readMessage(message* msg) {
    // Tableau pour stocker le contenu du message (90 est la taille maximale connue).
	pid_t sender = getMsgPID(msg);
    char* content = new char[90]; 
    int indice = 0;

    // Parcours du contenu jusqu'à la fin de la chaîne ou jusqu'à la taille maximale.
    for (int i = 10; i < 100 && msg->content[i] != '\0'; ++i) {
        content[indice++] = msg->content[i];
    }

    // Ajoute un caractère nul pour terminer la chaîne.
    content[indice] = '\0';

    return content;
}

// FIN DES METHODES EN DOUBLES

void readMessages(void* memory) {
    // Cast de la mémoire en un tableau de message
    message* messages = static_cast<message*>(memory);

    while(!stop) {
        sem_wait(semReceived);
        for (int i=0; i < MAX_MESS; ++i) {
            if (messages[i].type != FREE_MESSAGE_SPACE) {
                pid_t pidSender = getMsgPID(&messages[i]);
                char* content = readMessage(&messages[i]);
                // Une fois le message récupéré, on indique qu'il a déjà été traité :
                messages[i].type = FREE_MESSAGE_SPACE;

                // On peut procéder à l'affichage :
                cout << pidSender << " : " << content << endl;
                delete[] content;
            }
        }
        sem_post(semReceived);
    }
}

void readUserEntries(void* memory) {
    while(!stop) {
        // On récupère l'entrée dans une string
        string contentString;
        getline(cin, contentString);
        cout << endl;

        // On convertit cette stirng en char* :
        char* content = new char[contentString.size()+1];
        strcpy(content, contentString.c_str());

        // On appelle getUserMessage pour récupérer adapter le message au format accepté :
        char* messageContent = GetUserMessage(content);

        sem_wait(shmAcces);
        cout << "Envoie d'un message en cours ..." << endl;
        writeUserMessage(messageContent, memory, 0);
        sem_post(shmAcces);

        delete[] content;
        delete[] messageContent;
    }
}

void connect(void* memory) {
    cout << "System : " << "Sending connection message ..." << endl;
    writeUserMessage(strdup(to_string(getpid()).c_str()), memory, 1);
    cout << "System : " << " You are connected." << endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Le lancement d'un client nécessite 2 arguments.");
        perror("PID Serveur ..."); // Il faut quoi comme information pour lancer le client à part ça ?
        return -1;
    }

    /*
        Création de deux segments de mémoire partagés :
        - Emission : On récupère simplement la mémoire crée par le serveur afin d'y écrire nos messages
        - Reception : On crée ce segment, Le serveur y écrira.
    */

   // Emission :
   // argv[1] correspond au PID du serveur sous forme de chaine de caractère.
    int fd_emit = shm_open(argv[1], O_RDWR, 0666);
    if (fd_emit < 0) {
        perror("Erreur lors de shm_open (emit)");
        exit(EXIT_FAILURE);
    }

    void* shm = mmap(0, sizeof(myshm), PROT_READ | PROT_WRITE, MAP_SHARED, fd_emit, 0);
    if (shm == MAP_FAILED) {
        perror("Erreur lors du mapping memoire");
        close(fd_emit);
        EXIT_FAILURE;
    }

    // Temporaire pour éviter de devoir tout de suite simplifier les autres fonctions...
    sharedMemory = shm;

    // Réception :
    // Le client doit créer sa propre mémoire partagé que recevra les message du serveur.
    int fd_receive = shm_open(strdup(to_string(getpid()).c_str()), O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd_receive < 0) {
        perror("Erreur lors de shm_open (receive)");
        exit(EXIT_FAILURE);
    }

    // la taille de la mémoire partagé et de MAX_MESSAGE * la taille d'un message
    if (ftruncate(fd_receive, MAX_MESS * sizeof(message)) == -1) {
        perror("Erreur lors de frtuncate (receive)");
        close(fd_receive);
        exit(EXIT_FAILURE);
    }

    void* receivedMsg = mmap(0, MAX_MESS * sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, fd_receive, 0);
    if (receivedMsg == MAP_FAILED) {
        perror("Erreur lors de mmap (received)");
        close(fd_receive);
        exit(EXIT_FAILURE);
    }

    fillMessageSpace(receivedMsg);

    // Créations d'une sémaphore pour éviter lea accès concurent sur la mémoire des message reçus :
    std::string semReceivedName = "semReceived" + std::to_string(getpid());
    semReceived = sem_open(semReceivedName.c_str(), O_CREAT, 0666, 1);
    if (semReceived == SEM_FAILED) {
        perror("Erreur lors de sem_open pour semMax");
        exit(EXIT_FAILURE);
    }

        // Récupération du sémaphore pour la mémoire partagé du serveur :
    shmAcces = sem_open("shmAccess", 0); // On indique ici que le sémaphore doit déjà exister...
    if (shmAcces == SEM_FAILED) {
        perror("Erreur lors de sem_open pour shmAccess");
        exit(EXIT_FAILURE);
    }

    // Sémaphore pour notifier le serveur de l'arrivé d'un message :
    semServerReceivedMsg = sem_open("semReceived", 0); 
    if (semServerReceivedMsg == SEM_FAILED) {
        perror("Erreur lors de sem_open pour semServerReceivedMsg");
        exit(EXIT_FAILURE);
    }

    close(fd_emit);
    close(fd_receive);

    // Définition d'un handler :

    /*
        Instanciation des Threads :
        - Un thread pour lire les messages reçus
        - Un thread pour lire entrées utilisateur sur le flux standard et qui les envoies au serveur
    */
    threads.reserve(2);
    threads.emplace_back(thread(readMessages, receivedMsg));
    threads.emplace_back(thread(readUserEntries, shm));

    connect(shm);
    signal(SIGINT, handler);

    // AFFICHAGE DU PID DU CLIENT :
    cout << "CLIENT'S PID : " << getpid() << endl;

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}