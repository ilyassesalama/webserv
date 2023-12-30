#pragma once

#include "../../webserv.hpp"
#include "ConfigurationFile.hpp"

class Response {
    public:
        Response();
        
        void setRequest(RequestParser &request);
        void setServer(t_server &server);
        void setPath(std::string requestedResourcePath);
        void setRoute(t_route *route);
        void setResponseLine();
        void setHeaders();
        void setResponseBody();
        void setStatusCode(int code);
        void setServingState(bool status);
        void setBytesSent(size_t bytes);


		std::vector<char>& getResponse();
        bool isServing();
        std::string getStringStatus();
		// t_route * getSpecificRoute(std::string location);
        std::string getErrorPageHTML();
        std::string readFileByOffset();
        // std::string getAllowedMethod(std::string location, std::string method);


		void buildResourcePath(t_route *route);
        void GETResponseBuilder();
		void DELETEResponseBuilder();
		void POSTResponseBuilder();
        void responseBuilder();
        void clearResponse();

		void handleDirectoryRequest();
        void handleFileRequest();
		void autoIndexHTMLBuilder(size_t slashPos);
        void buildErrorResponse();
        void addDataToResponse(std::string data);
        void feedDataToTheSender();
        bool isLocationHasCGI();
        void CGIhandler();

        void uploadFile();
        void saveOnFile(std::string data);
        void handleboundaryStart(std::ifstream& inputfile);
        bool isUploading();

    private:
        std::string path;
        RequestParser *request;
        t_server *server;
        int statusCode;
        std::map<std::string, std::string> responseHeadersMap;
        std::string response;
        std::string responseLine;
        std::string responseHeaders;
        std::string responseBody; 
        std::vector<t_route>routes;
        std::vector<char>responseVector;
        int contentLength;
        std::string clientSidePath;
		t_route *currentRoute;
        size_t bytesToRead;
        std::streampos fileOffset;
        bool servingState;
        size_t bytesSent;
        std::string method;
        bool isCGI;

        std::streampos uploadFileOffset;
        bool uploading;
        std::string boundary;
        std::string boundaryFilePath;
        std::string uploadFilePath;

};

std::string generateHTMLStart(std::string path);
std::string createAnchor(std::string name);
std::string createParagraph(std::string lastUpdate);
std::string createSizeParagraph(long long size);
std::string createBodyHTML(std::string nameHTML, std::string lastUpdateHTML, std::string sizeHTML);
std::string generateHTMLEnd(std::string bodyHTML);