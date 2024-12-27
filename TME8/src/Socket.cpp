#include "Socket.h"
#include <netdb.h>      // Pour getaddrinfo
#include <unistd.h> // Pour close()
#include <iostream>
#include <arpa/inet.h>

namespace pr {
    void Socket::connect(const std::string& host, int port) {
        struct addrinfo* result;
        if (getaddrinfo(host.c_str(), NULL, NULL, &result) != 0) {
            perror("resolution DNS");
        }
	    struct in_addr ip;
        for (struct addrinfo* rp=result; rp != NULL; rp = rp->ai_next) {
            if (rp->ai_family == AF_INET) {
                ip = ((struct sockaddr_in*) rp->ai_addr)->sin_addr;
                break;
	    	}
	    }
        freeaddrinfo(result); // cleanup de la liste
        connect(ip, port);
    }

    void Socket::connect(in_addr ip, int port) {
        struct sockaddr_in addr; // Déclaration directe, pas de pointeur
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr = ip;

        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1) {
            perror("socket");
            return;
        }

        if (::connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("connect");
            ::close(fd);
            fd = -1;
            return;
        }
    }

    void Socket::close() {
        if (fd != -1) {
            // Tente de désactiver la lecture/écriture
            if (shutdown(fd, SHUT_RDWR) == -1) {
                perror("shutdown");
            }

            // Tente de fermer la socket
            if (::close(fd) == -1) {
                perror("close");
            }

            // Marque le descripteur comme invalide
            fd = -1;
        }
    }

    std::ostream & operator<< (std::ostream & os, struct sockaddr_in * addr) {
    if (!addr) {
        os << "Invalid address (nullptr)" << std::endl;
        return os;
    }

    os << "sin_family : " << addr->sin_family 
       << "; sin_port : " << ntohs(addr->sin_port) // Convertir en host byte order
       << "; sin_addr : " << inet_ntoa(addr->sin_addr) // Convertir en chaîne lisible
       << std::endl;

    return os;
}

}