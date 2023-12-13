#pragma once

#include "../../webserv.hpp"
#include "ConfigurationFile.hpp"

// #include"ServerInstance.hpp"
// class Response {
//     private:
//         RequestParser requestParser;
//         std::string response;
//         std::string path;
//         std::string contentType;
//         std::string contentSize;
//         int clientFD;
//         int status;
//     public:
        
//         Response(int clientFd, const RequestParser &parser);
    
//         std::string sendResponse();

//         void setPath(const std::string &path);

//         const int &getStatus();
//         const std::string &getResponse();
//         std::string getErrorPageHTML();
//         std::string getStringStatus();
// };


class Response {
    public:
        Response();
        ~Response();
        
        void setRequest(RequestParser &request);
        void setServer(t_server &server);
        void setPath(std::string path);
        void setResponseLine();
        void setHeaders();
        void setResponseBody();

        std::string getStringStatus();

        void setErrorResponse(int statusCode);
        size_t getContentLength();
        std::string getErrorPageHTML();
        void GETResponseBuilder();
        void responseBuilder();
        void clearResponse();
    private:
        std::string path;

        RequestParser *request;
        t_server *server;
        int statusCode;
        std::string response;
        std::string responseLine;
        std::string responseHeaders;
        std::string responseBody; 
        int contentLength;
};


// headers in response : 
// content-Type / content-length / Transfer-encoding / connection 