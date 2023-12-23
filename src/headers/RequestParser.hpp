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
        ParsingState parsingState;
        std::string requestResourcePath;
        std::string requestData;
        std::string body;
        std::map<std::string, std::string> requestLine;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;
		t_server *server;
        t_route *route;
		size_t chunkRemainder;

		std::string boundary;
		bool isChunked;
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

		void setServerInformation(t_server *server);

        t_route *getRoute();
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

		void getChunkedData(std::string &body);
		void getBoundary(std::string contentType);
		void getBoundaryContent(std::string &body);
		bool parseContentType(); // add by abahsine
};