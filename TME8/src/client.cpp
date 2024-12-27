#include "ServerSocket.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include "Socket.h"
#include "FTPHandler.h"

int main00() {
	pr::Socket sock;
	sock.connect("localhost", 1664);
	int N=42;
	write(sock.getFD(),&N,sizeof(int));
	read(sock.getFD(),&N,sizeof(int));
	std::cout << N << std::endl;
	return 0;
}


// avec controle
int main0() {

	pr::Socket sock;

	sock.connect("localhost", 1664);

	if (sock.isOpen()) {
		int fd = sock.getFD();
		int i = 10;
		ssize_t msz = sizeof(int);
		if (write(fd, &i, msz) < msz) {
			perror("write");
		}
		std::cout << "envoyé =" << i << std::endl;
		int lu;
		auto nblu = read(fd, &lu, msz);
		if (nblu == 0) {
			std::cout << "Fin connexion par serveur" << std::endl;
		} else if (nblu < msz) {
			perror("read");
		}
		std::cout << "lu =" << lu << std::endl;
	}

	return 0;
}


// avec une boucle, on attend un 0
int main01() {

	pr::Socket sock;

	sock.connect("localhost", 1664);

	if (sock.isOpen()) {
		int fd = sock.getFD();

		ssize_t msz = sizeof(int);
		for (int i = 10; i >= 0; i--) {
			if (write(fd, &i, msz) < msz) {
				perror("write");
				break;
			}
			std::cout << "envoyé =" << i << std::endl;

			int lu;
			auto nblu = read(fd, &lu, msz);
			if (nblu == 0) {
				std::cout << "Fin connexion par serveur" << std::endl;
				break;
			} else if (nblu < msz) {
				perror("read");
				break;
			}
			std::cout << "lu =" << lu << std::endl;
		}
	}

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		perror("Echec du lancement du client : Pas assez d'arguments (IP, port)");
		return -1;
	}
	const char* ip = argv[1];
	int port = std::atoi(argv[2]);

	pr::Socket sock;
	sock.connect(ip, port);
	
	// Balises :
    const char* list = "#LIST";
    const char* upload = "#UPLOAD_";
    const char* download = "#DOWNLOAD_"; // "_" indique le début du prochain argument

	if (sock.isOpen()) {
		int fd = sock.getFD();
		std::cout << "La socket est ouverte." << std::endl;
		while (true) {
			std::string inputStr;
			std::getline(std::cin, inputStr);
			const char* cInput = inputStr.c_str(); // On transforme en char* pour directement envoyer au serveur
			if(strstr(cInput, list) == cInput) { // La chaine commence par list
				// Balises listes : 
		        const char* baliseDebut = "#BeginList";
        		const char* baliseFin = "#EndList";

				std::cout << "Envoie d'une demande de listage." << std::endl;
				if (write(fd, cInput, strlen(cInput)) < strlen(cInput)) {
					perror("Erreur d'écriture lors de l'envoie d'un message au serveur.");
					break;
				}
				while(true) {
					char buff[1024];
					if (read(fd, buff, sizeof(buff))) {
						if (strstr(buff, baliseDebut) == buff) {
							std::cout << "Demande acceptée, début du listage ..." << std::endl;
						}
						else if (strstr(buff, baliseFin) == buff) {
							std::cout << "Fin du listage." << std::endl;
							break;
						}
						else {
							std::cout << buff << std::endl;
						}
					}
				}

			// FIN listage

			} else if (strstr(cInput, upload) == cInput) {
				std::cout << "Envoie d'une demande d'upload." << std::endl;
				if (write(fd, cInput, strlen(cInput)) < strlen(cInput)) {
					perror("Erreur d'écriture lors de l'envoie d'un message au serveur.");
					break;
				}	

		        const char* baliseDebut = "#BeginFile";
		        const char* baliseFin = "#EndFile";
        		const char* baliseSendFile = "#SendFile";
        		const char* baliseFileReceived = "#ReceivedFile";

				std::cout << "Emission de la demande d'envoie d'un fichier ..." << std::endl;
				if (write(fd, baliseDebut, strlen(baliseDebut)) < strlen(baliseDebut)) {
					perror("Erreur d'écriture lors de la demande d'envoie du fichier au serveur.");
					break;
				}
				
				char buff[1024]; // Il est nécessaire de vier le tableau ?
				read(fd, buff, sizeof(buff));
				if (strstr(buff, baliseSendFile) == buff) {
					std::cout << "Le serveur est prêt à recevoir le fichier." << std::endl;
					std::cout << "Début de l'envoie ..." << std::endl;
				}
				
				// Préparation du fichier à upload
				char* name = pr::findArguments(cInput)[0]; // On récupère le nom
				FILE* file = fopen(name, "rb"); // On ouvre le fichier en mode lecture binaire.
				if (file == nullptr) {
					perror("Echec de l'ouverture du fichier en lecture binaire.");
					return;
				}

		        size_t bytesRead;
        		bool sending = false;

				while(true) {
					buff[1024];
					if (read(fd, buff, sizeof(buff))) {
						if (strstr(buff, baliseFileReceived) == buff) {
							std::cout << "Le fichier a été reçu, fin de la transmission." << std::endl;
							break;
						}
						else { // Envoie du fichier dans son intégralité :
							while((bytesRead = fread(buff, 1, sizeof(buff), file)) > 0) {
								if(write(fd, buff, bytesRead) < 0) {
									if(write(fd, buff, bytesRead) < 0) {
										perror("Erreur lors de l'envoie du fichier.");
										fclose(file);
										return;
									}
								}
							}
							fclose(file);
							if (write(fd, baliseFin, strlen(baliseFin)) < 0) {
								perror("Echec de l'envoie du message de confirmation de la fin de l'envoie.");
								break;
							}
							std::cout << "Fin de l'upload." << std::endl;
							std::cout << "Attente du message de confirmation ..." << std::endl;
						}
					}
				}
			} 
			
			// FIN upload

			else if (strstr(cInput, download) == download) {
				std::cout << "Envoie d'une demande de download." << std::endl;
				if (write(fd, cInput, strlen(cInput)) < strlen(cInput)) {
					perror("Erreur d'écriture lors de l'envoie d'un message au serveur.");
					break;
				}			}
		}
	}


	else {
		perror("Erreur : Echec de l'ouverture de la socket.");
		sock.close();
	}
}
