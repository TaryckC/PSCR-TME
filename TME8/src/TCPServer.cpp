#include "TCPServer.h"
#include "ServerSocket.h"
#include <iostream>
#include <thread>

namespace pr {
    
    bool TCPServer::startServer (int port) {
        if (ss == nullptr) {
            ss = new ServerSocket(port);
            if (!(ss->isOpen())) {
                std::cerr << "Erreur lors de la création du serveur d'attente." << std::endl;
                delete ss;
                ss = nullptr;
                return false;
            }
            std::cout << "Un serveur d'attente a bien été crée." << std::endl;
            return true;
        }
        std::cout << "Il existe déjà un server d'attente en fonctionnement." << std::endl;
        return false;
        while(true) {
            Socket clientSocket = ss->accept(); //Appelle bloquant
            
            // Création d'un thread
            threads.push_back(std::thread([this, clientSocket = std::move(clientSocket)]() mutable {
                this->job(clientSocket); // Appelle la méthode membre job
            }));
        }
    }

    void TCPServer::stopServer () {
        if (ss != nullptr) {
            for (std::thread& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
            threads.clear();
            delete ss;
            ss = nullptr;
            std::cout << "Le serveur d'attente a été arrêter." << std::endl;
            return;
        }
        std::cout << "Aucun serveur n'est en fonctionnement." << std::endl;
    }

    void TCPServer::job(Socket socket) {
        ConnectionHandler* handlerClone = handler->clone();
        handlerClone->handleConnection(socket);
        delete(handlerClone);
    }

} 