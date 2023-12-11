#pragma once

#include "libs.hpp"
#include "RequestParser.hpp"
#include "logger.hpp"
#include "File.hpp"

class Response {
    private:
        RequestParser requestParser;
        std::string response;
        std::string path;
        std::string contentType;
        std::string contentSize;
        int clientFD;
        int status;
    public:
        Response(int clientFd, const RequestParser &parser);
    
        void sendResponse();

        void setPath(const std::string &path);

        const int &getStatus();
        const std::string &getResponse();
        std::string getErrorPageHTML();
        std::string getStringStatus();
};