#include "FTPHandler.h"
#include <dirent.h>
#include <unistd.h>
#include <iostream>

namespace pr {
    char** findArguments(const char* buff) {
        if (buff == nullptr || strlen(buff) == 0) {
            std::cerr << "Buffer vide ou invalide." << std::endl;
            return nullptr;
        }

        int nbArguments = 0;
        size_t i = 0;

        // Étape 1 : Compter les arguments
        while (buff[i]) {
            if (buff[i] == '_') {
                ++nbArguments;
            }
            ++i;
        }

        // Si aucun argument trouvé, retourner un tableau vide
        if (nbArguments == 0) {
            return nullptr;
        }

        // Étape 2 : Allouer un tableau pour les arguments
        char** res = new char*[nbArguments];
        size_t start = 0; // Début du prochain argument
        int argIndex = 0;

        // Étape 3 : Extraire les arguments
        for (i = 0; buff[i]; ++i) {
            if (buff[i] == '_') {
                size_t len = i - start; // Longueur de l'argument actuel
                res[argIndex] = new char[len + 1];
                strncpy(res[argIndex], &buff[start], len);
                res[argIndex][len] = '\0'; // Null-terminer la chaîne
                ++argIndex;
                start = i + 1; // Début du prochain argument
            }
        }

        return res;
    }

    FTPHandler* FTPHandler::clone() const {
        return new FTPHandler(*this);
    }

void FTPHandler::handleConnection(Socket s) {
    // Balises :
    const char* list = "#LIST";
    const char* upload = "#UPLOAD_";
    const char* download = "#DOWNLOAD_"; // "_" indique le début du prochain argument

    int fd = s.getFD();
    char buff[1024];

    // Boucle principale pour gérer les requêtes
    while (true) {
        ssize_t bytesRead = read(fd, buff, sizeof(buff) - 1); // Laisse de la place pour le null-terminateur
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Connexion fermée par le client." << std::endl;
            } else {
                perror("Echec de la lecture des arguments.");
            }
            return; // Quitte la méthode en cas d'erreur ou de déconnexion
        }

        // Null-terminer le buffer pour éviter des erreurs avec strstr
        buff[bytesRead] = '\0';

        // Traitement des commandes :
        if (strstr(buff, list) == buff) { // Vérifie que #LIST est au début
            listFiles(s);
        } else if (strstr(buff, upload) == buff) { // Vérifie que #UPLOAD_ est au début
            char** args = findArguments(buff);
            if (args != nullptr && args[0] != nullptr) {
                uploadFile(s, args[0]);
                delete[] args[0];
                delete[] args;
            } else {
                std::cerr << "Aucun argument fourni pour UPLOAD." << std::endl;
            }
        } else if (strstr(buff, download) == buff) { // Vérifie que #DOWNLOAD_ est au début
            char** args = findArguments(buff);
            if (args != nullptr && args[0] != nullptr) {
                downloadFile(s, args[0]);
                delete[] args[0];
                delete[] args;
            } else {
                std::cerr << "Aucun argument fourni pour DOWNLOAD." << std::endl;
            }
        } else {
            std::cerr << "Commande non reconnue : " << buff << std::endl;
        }
    }
}

    void FTPHandler::listFiles(Socket s) {
        DIR* dir = opendir(filename);
        if (dir == nullptr) {
            perror("opendir échec");
            return;
        }

        struct dirent* entry;
        int fd = s.getFD();

        const char* baliseDebut = "#BeginList";
        const char* baliseFin = "#EndList";

        ssize_t msz = strlen(baliseDebut);
        if(write(fd, baliseDebut, msz)< msz) {
            perror("Erreur d'envoie de la balise : Begin");
        }

        std::cout << "Éléments du répertoires : " << std::endl;
        while ((entry = readdir(dir)) != nullptr) {
            // TODO : Pour chacune des entrées du DIR -> Envoyer le nom du fichier au client
            const char* fileName = entry->d_name;
            msz = strlen(fileName);
            std::cout << fileName << "Status ";
            if (write(fd, fileName, msz) < msz) {
                std::cout << " : /!\\ Échec de l'envoie." << std::endl;
            }
            else {
                std::cout << " : Envoyé. " << std::endl;
            }
        }
        msz = strlen(baliseFin);
        if (write(fd, baliseFin, msz) < msz) {
            perror("Erreur d'envoie de la balise : End");
        }

        closedir(dir); // Fermeture du répertoire
    }

    void FTPHandler::uploadFile(Socket s, const char* name) {
        DIR* dir = opendir(filename);
        if (dir == nullptr) {
            perror("opendir échec");
            return;
        }
        int fd = s.getFD();
        if (fd < 0) {
            std::cerr << "Descripteur de socket invaldie." << std::endl;
            return;
        }
        const char* baliseDebut = "#BeginFile";
        const char* baliseFin = "#EndFile";
        const char* baliseSendFile = "#SendFile";
        const char* baliseFileReceived = "#ReceivedFile";

        FILE* newFile = fopen(name, "wb"); // On ouvre le fichier en mode écriture binaire.
        if (newFile == nullptr) {
            perror("Erreur lors de la création du fichier.");
            return;
        }

        // Mise en place du nécessaire pour la lecture du fichier entrant :
        char buffer[1024];
        bool receiving = false;
        // On suppose que les balises sont envoyées dans des messages individuels
        // Attente de la balise de début : 
        while (true) {
            ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
            if (bytesRead <= 0) {
                if (bytesRead == 0) {
                    std::cerr << "Fin de connexion client." << std::endl;
                }
                else {
                    perror("Erreur de reception");
                }
                fclose(newFile);
                return;
            }

            buffer[bytesRead] = '\0';

            if (receiving) {
                if (fwrite(buffer, 1, bytesRead, newFile)) {
                    perror("Erreur d'écriture dans le fichier.");
                    break;
                }
            }

            if (receiving && strcmp(buffer, baliseFin) == 0) {
                std::cout << "Fichier reçu avec succès." << std::endl;
                std::cout << "Envoie d'un message de confirmation de l'envoie." << std::endl;
                if (write(fd, baliseFileReceived, strlen(baliseFileReceived)) < 0) {
                    perror("Erreur d'envoi de la confirmation de la réception.");
                    return;
                }
                break;
            }

            if (!receiving && strcmp(buffer, baliseDebut) == 0) {
                std::cout << "La demande d'upload a été reçue. Renvoie d'un message OK." << std::endl;
                if (write(fd, baliseSendFile, strlen(baliseSendFile)) < 0) {
                    perror("Erreur d'envoi de la confirmation 'OK'");
                    return;
                }
                
                std::cout << "Le fichier est maintenant en cours d'écriture." << std::endl;
                receiving = true;
                continue;
            }
        }
        fclose(newFile);
    }

    void FTPHandler::downloadFile(Socket s, const char* name) {
        DIR* dir = opendir(filename);
        if (dir == nullptr) {
            perror("Echec ouverture dir");
            return;
        }

        const char* baliseDebut = "#BeginFile";
        const char* baliseFin = "#EndFile";
        const char* baliseSendFile = "#SendFile";
        const char* baliseFileReceived = "#ReceivedFile";

        int fd = s.getFD();
        if (fd < 0) {
            std::cerr << "Descripteur de socket invaldie." << std::endl;
            return;
        }
        
        FILE* file = fopen(name, "rb"); // On ouvre le fichier en mode lecture binaire.
        if (file == nullptr) {
            perror("Echec de l'ouverture du fichier en lecture binaire.");
            return;
        }

        char buffer[1024];
        size_t bytesRead;
        bool sending = false;

        // Envoie de la balise de début d'envoie :
        std::cout << "Annonce de l'envoie du fichier." << std::endl;
        if(write(fd, baliseDebut, strlen(baliseDebut)) < 0) {
            perror("Echec de l'envoie du fichier (baliseDebut)");
            fclose(file);
            return;
        }

        while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            if(write(fd, buffer, bytesRead) < 0) {
                if(write(fd, buffer, bytesRead) < 0) {
                    perror("Erreur lors de l'envoie du fichier.");
                    fclose(file);
                    return;
                }
            }
        }

        // Vérifier si une erreur est survenue pendant la lecture
        if (ferror(file)) {
            perror("Erreur de lecture du fichier.");
        }

        std::cout << "Annonce de la fin de l'envoie du fichier." << std::endl;
        if(write(fd, baliseFin, strlen(baliseFin)) < 0) {
            perror("Echec de l'envoie du fichier (baliseFin)");
            fclose(file);
            return;
        }
        fclose(file);
        std::cout << "Fichier envoyé avec succès : " << name << std::endl;
    }

} 