#pragma

#include "../../../webserv.hpp"

class Response {
    private:
        std::string response;
        int status;

        std::string path;
    public:
        Response();
        Response(const std::string &path);

        void sendResponse(int &clientFD);
        void setPath(const std::string &path);
        const std::string &getResponse();
        const int &getStatus();
};