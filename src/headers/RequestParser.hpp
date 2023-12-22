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
        t_route *route;

		bool isChunked;
		size_t chunkRemainder;
    public:
        RequestParser();

        void mergeRequestChunks(std::string &receivedData);
        void nullOutVars();
        void logParsedRequest();

        void parseRequestLine(std::string &requestData);
        void parseRequestHeaders(std::string &requestData);
        void parseRequestParams(std::string &requestData);
        void parseRequestBody(std::string &requestData);
        void verifyIfRequestIsSafe();

        t_route *getRoute();
		void setServerInformation(t_server *server);
        std::map<std::string, std::string> &getRequestLine();
        std::string getRequestedResourcePath();
        std::map<std::string, std::string> &getHeaders();
        const std::map<std::string, std::string> &getParams();
        const std::string &getBody();
        const ParsingState &getParsingState();
        std::string& getRequestData();
        bool isPathAccessible();
        bool isMethodAllowed();
        bool isHeaderLineValid();
		bool parseContentType(); // add by abahsine


		void getChunkedData(std::string &body);
};