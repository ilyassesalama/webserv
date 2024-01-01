#pragma once

#include "../../webserv.hpp"
#include "RequestParser.hpp"


class CGInstance {
    public:
        CGInstance(RequestParser &request);

        void setFilePath(std::string filePath);
        void setCGIPath(std::string cgiPath);
        void setEnvironnementVariables();
        void setCGIServer();
        void setRequestBody();

        void initCGInstance();
        void executeScript();
        void parseResponseHeaders();
        void parseResponseBody();
        void printCGIResponse();

        std::string getCGIResponse();
        std::string getCGIContentType();
        std::string getQueryString(std::map<std::string, std::string> queries);
        size_t getCGIContentLength();
        int getCGIStatusCode();
    private:
        RequestParser &request;
        std::string filePath;
        std::string cgiPath;
        std::string cgiResponse;
        std::string cgiResponseHeaders;
        std::string cgiContentType;
        std::string cgiStatusCode;
        std::string cgiServer;
        std::string cgiRequestBody;
        std::string cgiRequestBodySize;
        std::map<std::string, std::string> cgiResponseHeadersMap;
        char **cgiEnv;
};


// env 
// REQUEST_METHOD: The HTTP request method (e.g., "GET" or "POST").
// QUERY_STRING: The query string portion of the URL.
// CONTENT_TYPE: The content type of the request body for POST requests.
// CONTENT_LENGTH: The length of the request body for POST requests.
// SCRIPT_FILENAME: The absolute path to the PHP script file.
// SCRIPT_NAME: The virtual path of the script.
// DOCUMENT_ROOT: The document root directory.
// SERVER_NAME: The server's hostname.
// SERVER_PORT: The server's port number.