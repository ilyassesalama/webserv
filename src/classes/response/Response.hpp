#pragma

#include "../../../webserv.hpp"

class Response {
    private:
        int clientFD;
        std::string response;
        int status;

        std::string path;
    public:
        Response(int clientFd, const std::string &path);
    
        void sendResponse();
        std::string getErrorPageHTML();
        void setPath(const std::string &path);
        const std::string &getResponse();
        const int &getStatus();
        std::string getStringStatus();
};