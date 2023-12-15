#pragma once

#include "../../webserv.hpp"
#include "ConfigurationFile.hpp"

typedef struct ParsingState {
    bool headLineOk;
    bool headsOk;
    bool bodyOk;
    bool ok;
    int failCode;
    std::string failReason;
} ParsingState;

class RequestParser {
    private:
        std::string requestData;
        ParsingState parsingState;
        std::map<std::string, std::string> requestLine;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;
        std::string body;
		t_server *server;
        std::string requestResourcePath;
    public:
        RequestParser();

        void mergeRequestChunks(std::string &receivedData);
        void nullOutVars();
        void logParsedRequest();

        void setParsingState(ParsingState state);
        void parseRequestLine(std::string &requestData);
        void parseRequestHeaders(std::string &requestData);
        void parseRequestParams(std::string &requestData);
        void parseRequestBody(std::string &requestData);
        void parseFinalRequest();

		void setServerInformation(t_server *server);
        std::map<std::string, std::string> &getRequestLine();
        std::map<std::string, std::string> &getHeaders();
        const std::map<std::string, std::string> &getParams();
        const std::string &getBody();
        const ParsingState &getParsingState();
        std::string& getRequestData();
        bool isPathAccessible();
        bool isMethodAllowed();
};