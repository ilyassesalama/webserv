#pragma once

#include "../../webserv.hpp"
#include "RequestParser.hpp"


class CGInstance {
    public:
        CGInstance(RequestParser &request);

        void setFilePath(const std::string &filePath);
        void setCGIPath(std::string cgiPath);
        void setEnvironnementVariables();
        void setCGIServer();
        void setRequestBody();

        void initCGInstance();
        void executeScript();
        void parseResponseHeaders();
        void parseResponseBody();
        void printCGIResponse();
        void saveSessionCookie();

        std::string getCGIResponse();
        std::string getCGIContentType();
        std::map<std::string, std::string> getCGIResponseHeadersMap();
        std::string getQueryString(std::map<std::string, std::string> queries);
        size_t getCGIContentLength();
        int getCGIStatusCode();
        bool hasCGIFailed();
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
        bool cgiFailureStatus;
};