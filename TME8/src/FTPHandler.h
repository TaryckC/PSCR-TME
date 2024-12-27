#include "ConnectionHandler.h"

namespace pr {
    class FTPHandler : public ConnectionHandler {
        // Attributs :
        const char* filename;
    public :
        // Méthodes :
        FTPHandler* clone() const override;
        void handleConnection(Socket s) override;

        void listFiles(Socket s);
        void uploadFile(Socket s, const char* name);
        void downloadFile(Socket s, const char* name);
    };

    char** findArguments(const char* buff);
}