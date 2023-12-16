#pragma once

#include "../../webserv.hpp"
#include "ConfigurationFile.hpp"

class Response {
    public:
        Response();
        ~Response();
        
        void setRequest(RequestParser &request);
        void setServer(t_server &server);
        void setPath(std::string path, std::string method);
        void setRoute();
        void setResponseLine();
        void setHeaders();
        void setResponseBody();
        void setStatusCode(int code);

		std::string getResponse();
        std::string getStringStatus();
		t_route * getSpecificRoute(std::string location);
        std::string getErrorPageHTML();
        std::string getAllowedMethod(std::string location, std::string method);


		void buildResourcePath(t_route *route);
        void GETResponseBuilder();
		void POSTResponseBuilder();
        void responseBuilder();
        void clearResponse();

		void handleDirectoryRequest();
        void handleFileRequest();
		void autoIndexHTMLBuilder(std::string indexHTML);
    private:
        std::string path;
        RequestParser *request;
        t_server *server;
        int statusCode;
        std::string response;
        std::string responseLine;
        std::string responseHeaders;
        std::string responseBody; 
        std::vector<t_route>routes;
        int contentLength;
        std::string clientSidePath;
		t_route *currentRoute;
};

std::string generateHTMLStart(std::string path);
std::string createAnchor(std::string name);
std::string createParagraph(std::string lastUpdate);
std::string createSizeParagraph(long long size);
std::string createBodyHTML(std::string nameHTML, std::string lastUpdateHTML, std::string sizeHTML);
std::string generateHTMLEnd(std::string bodyHTML);

std::string getContentType(std::map<std::string, std::string>&headers, std::string path);
