#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>



// -------- start request-related code -------
class RequestParser {
    private:
        std::map<std::string, std::string> requestLine;
        std::map<std::string, std::string> headers;
        std::string body;
    public:
        RequestParser();
        void initRequestParser(std::string &requestData);
        void parseRequestLine(std::string &requestData);
        void parseRequestHeaders(std::string &requestData);
        void parseRequestBody(std::stringstream &httpStream);

        const std::map<std::string, std::string> &getRequestLine();
        const std::map<std::string, std::string> &getHeaders();
        const std::string &getBody();

        void setRequestLine(std::map<std::string, std::string> requestLine);
        void setHeaders(std::map<std::string, std::string> headers);
        void setBody(std::string body);
};
// -------- end request-related code -------



#endif