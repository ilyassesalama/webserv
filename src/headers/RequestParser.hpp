#pragma once

#include "../../webserv.hpp"

// -------- start request-related code -------
class RequestParser {
    private:
        std::map<std::string, std::string> requestLine;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;
        std::string body;
    public:
        RequestParser();
        void initRequestParser(std::string &requestData);
        void parseRequestLine(std::string &requestData);
        void parseRequestHeaders(std::string &requestData);
        void parseRequestParams(std::string &requestData);
        void parseRequestBody(std::stringstream &httpStream);

        const std::map<std::string, std::string> &getRequestLine();
        const std::map<std::string, std::string> &getHeaders();
        const std::map<std::string, std::string> &getParams();
        const std::string &getBody();

        void setRequestLine(std::map<std::string, std::string> requestLine);
        void setHeaders(std::map<std::string, std::string> headers);
        void setParams(std::map<std::string, std::string> headers);
        void setBody(std::string body);
};
// -------- end request-related code -------