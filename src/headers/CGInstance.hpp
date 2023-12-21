#pragma once

#include "../../webserv.hpp"
#include"RequestParser.hpp"


class CGInstance {
    public:
        CGInstance();
        // ~CGInstance();
        void setPath(std::string path);
        void setcgiPath(std::string cgiPath);
        void setEnvironnementVariables(RequestParser *parser);
        // void executeScript(std::vector<std::string>& CGIenv);
    private:
        std::string path; // going to be constructed with
        std::string cgiPath;
        char** env;

        //env
         std::string REQUEST_METHOD;
         std::string QUERY_STRING;
         std::string SCRIPT_FILENAME;
         std::string SCRIPT_NAME;
         std::string DOCUMENT_ROOT;
         std::string SERVER_NAME;
         std::string SERVER_PORT;
         std::string REDIRECT_STATUS;
};


//env 
// REQUEST_METHOD: The HTTP request method (e.g., "GET" or "POST").
// QUERY_STRING: The query string portion of the URL.
// CONTENT_TYPE: The content type of the request body for POST requests.
// CONTENT_LENGTH: The length of the request body for POST requests.
// SCRIPT_FILENAME: The absolute path to the PHP script file.
// SCRIPT_NAME: The virtual path of the script.
// DOCUMENT_ROOT: The document root directory.
// SERVER_NAME: The server's hostname.
// SERVER_PORT: The server's port number.