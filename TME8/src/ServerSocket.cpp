#include "ServerSocket.h"
#include <iostream>
#include <signal.h>
#include <unistd.h> // Pour close()

namespace pr {


    ServerSocket* currentServer = nullptr; // Déclaration globale

    void handler(int sig) {
        if (currentServer) {
            currentServer->close(); // Utilise l'instance actuelle pour fermer le socket
            std::cout << "Server stopped due to signal " << sig << std::endl;
        }
        exit(sig); // Termine le programme après avoir traité le signal
    }

    ServerSocket::ServerSocket(int port) {
        socketfd = -1;

        // Gestion des signaux :
        signal(SIGINT, pr::handler);
        pr::currentServer = this;
        // FIN Gestion des singaux

        // Serveur => socket, bin*, listen
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            ::close(socketfd);
            socketfd = -1;
            return;
        }
        if (listen(socketfd, 10) < 0) { // 10 indique le nombre de personne maximum pouvant être écouté
            perror("listen");
            ::close(socketfd);
            socketfd = -1;
            return;
        }
    }

    Socket ServerSocket::accept() {
        if (!isOpen()) {
            return Socket();
        }
        struct sockaddr_in exp;
        socklen_t len;
        int fdcom = ::accept(socketfd, (struct sockaddr*)&exp, &len);
        if (fdcom < 0) {
            perror("accept");
            return Socket();
        }
        std::cout << "Incoming connection" << &exp << std::endl;
        return Socket(fdcom);
    }

    void ServerSocket::close() {
        if (socketfd != -1) {
            shutdown(socketfd, 2); // socketfd, SHUT_RD = 0 | SHUT_WR = 1 | SHUT_UP = 2 
            ::close(socketfd);
            socketfd = -1;
        }
    }

}