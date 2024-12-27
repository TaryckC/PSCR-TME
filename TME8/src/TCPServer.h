#ifndef SRC_TCPSERVER_H_
#define SRC_TCPSERVER_H_

#include <thread>
#include "ServerSocket.h"
#include "ConnectionHandler.h"
#include <vector>

namespace pr {

// un serveur TCP, la gestion des connections est déléguée
class TCPServer {
	ServerSocket * ss; // la socket d'attente si elle est instanciee
	ConnectionHandler * handler; // le gestionnaire de session passe a la constru
	// a completer
	std::vector<std::thread> threads;
public :
	TCPServer(ConnectionHandler * handler): ss(nullptr),handler(handler) {}
	// Tente de creer une socket d'attente sur le port donné
	bool startServer (int port);

	// stoppe le serveur
	void stopServer () ;

	//Job des threads :
	virtual void job(Socket socket);
};

} // ns pr

#endif /* SRC_TCPSERVER_H_ */
