#include "TCPServer.h"

namespace pr {
    class FTPServer : public TCPServer {
        const char* baseDir;
    public :
        FTPServer(ConnectionHandler * handler, const char* baseDir): TCPServer(handler), baseDir(baseDir) {}
        const std::string getbaseDir() { return baseDir;}
    };
}