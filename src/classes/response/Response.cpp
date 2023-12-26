#include "../../../webserv.hpp"

Response::Response() {
    this->clientSidePath = File::getWorkingDir();
    this->path = "";
    this->server = NULL;
    this->request = NULL;
    this->response = "";
    this->responseLine = "";
    this->responseHeaders = "";
    this->responseBody = "";
    this->statusCode = 0;
    this->bytesToRead = 1024;
    this->fileOffset = 0;
    this->bytesSent = 0;
    this->servingState = false;
    this->uploadFileOffset = 0;
}


Response::~Response() {}

void Response::feedDataToTheSender() {
    if(fileOffset == -2 &&  bytesSent == responseVector.size()) {
        setServingState(false);
        return;
    }
    if(fileOffset == -1 && bytesSent == responseVector.size()) {   
        //finished file : change the serving status so he can recv another requestes
        setServingState(false);
        return;
    }
    if(bytesSent < responseVector.size()) {
        std::vector<char>tmp(responseVector.begin() + bytesSent,responseVector.end());
        this->responseVector = tmp;
        return;
    }
    if(bytesSent == responseVector.size()) {
        bytesSent = 0;
        responseVector.clear();
        try {
            addDataToResponse(this->readFileByOffset());
        } catch (Utils::WebservException &ex) {
            addDataToResponse("");
        }
    }
}

void Response::responseBuilder() {
   try {
        // check if the parser failed
        if(this->statusCode != 200 && this->statusCode != 201){
			throw(Utils::WebservException("ResponseBuilder: Parser failed to parse the request"));
            return;
        }
        if (this->request->getRequestLine()["method"] == "GET") {
            GETResponseBuilder();
            return;
        }
        if (this->request->getRequestLine()["method"] == "DELETE") {
            DELETEResponseBuilder();
            return;
        }
        if (this->request->getRequestLine()["method"] == "POST") {
            POSTResponseBuilder();
            return;
        }
    } catch(Utils::WebservException &ex) {
        buildErrorResponse();
    }
}


/*
	Builds and adds the error page HTML source code to the response body
    to the response verctor to get ready for sending as a response.
*/
void Response::buildErrorResponse() {
    this->responseBody = getErrorPageHTML();
    this->setHeaders();
    this->setResponseLine();
    this->response.append(this->responseLine);
    this->response.append(this->responseHeaders);
    this->response.append(this->responseBody);
    addDataToResponse(response);
}

/**
 * @brief Iterates through the error pages vector and returns the path of the
 * error page that matches the status code.
 * 
 * @return std::string: the path of the error page corresponding to the status code.
 */
std::string getErrorPagePath(std::vector<t_error_page> &pages, int errorCode) {
	std::vector<t_error_page>::iterator it;

	for (it = pages.begin(); it != pages.end(); it++)
		if (it->error_code == errorCode)
			return it->error_page;
	for (it = pages.begin(); it != pages.end(); it++)
		if (it->error_code == 500)
			return it->error_page;
	return NULL;
}


/**
 * @brief Responsible for getting error pages path by status code.
 * In case of failure, this will return 403 error as a basic HTML page.
 * 
 * @return std::string: the HTML source code of the error page.
 */
std::string Response::getErrorPageHTML(){
    std::string responseBody;
	std::string error_page;

    this->path = getErrorPagePath(this->server->error_pages, this->statusCode);

    try {
        responseBody = File::getFileContent(this->path);
    } catch (std::exception &e) {
        responseBody = "<html><body><h1>500 | Server Internal Error</h1><p>It seems like our awesome webserv couldn't handle your request. Falling back to error 500 since no solution can be proposed in this case.</p></body></html>";
    }
    return responseBody;
}

void Response::handleDirectoryRequest() {

    size_t slashPos = this->path.find_last_of("/");

    std::string indexHTML = slashPos != this->path.size() - 1 ? "/index.html" : "index.html";

    if (File::isFile(this->path + indexHTML)) {
		this->path += indexHTML;
        this->responseBody = readFileByOffset();
		this->statusCode = 200;
    } else {
        bool directory_listing = this->currentRoute->directory_listing;

        if (directory_listing) {
            // response should be list of files in the directory
			this->autoIndexHTMLBuilder(slashPos);
        } else {
            this->statusCode = 403;
            this->responseBody = this->getErrorPageHTML();
        }
    }
}

std::string Response::readFileByOffset() {
    char buffer[this->bytesToRead];
    std::string content;
    std::ifstream file(this->path.c_str(),std::ios::binary);

    if(!file.is_open()) {
        throw(Utils::WebservException("readFileByOffset: Can't open \"" + this->path + "\" due to " + std::string(strerror(errno))));
    }
    file.seekg(this->fileOffset);
    file.read(buffer,this->bytesToRead);
    this->fileOffset = file.tellg();

    content.append(buffer,file.gcount());
    return(content);
}

void Response::CGIhandler() {
    CGInstance cgiHandler(*this->request);
    cgiHandler.initCGInstance(); // start the party

    // GCI finished doing the cool stuff
    this->responseBody = cgiHandler.getCGIResponse();
    this->statusCode = cgiHandler.getCGIStatusCode();
    this->responseHeadersMap["Content-Type"] = cgiHandler.getCGIContentType();
    this->responseHeadersMap["Content-Length"] = String::to_string(cgiHandler.getCGIContentLength());
}

void Response::handleFileRequest() {
    this->isCGI = this->isLocationHasCGI();
    if(this->isCGI){
        CGIhandler();
        return;
    }
    this->responseBody = readFileByOffset();
}

void Response::clearResponse() {
    this->path = "";
    this->response = "";
    this->responseLine = "";
    this->responseHeaders = "";
    this->responseBody = "";
    this->statusCode = 0;
	this->clientSidePath = File::getWorkingDir();
    this->responseVector.clear();
    this->fileOffset = 0;
    this->servingState = false;
    this->bytesSent = 0;
}

void Response::buildResourcePath(t_route *route) {
    std::string requestedResource;

    requestedResource.append(this->clientSidePath);
    requestedResource.append(route->root);
    requestedResource.append(this->path);
    this->path = requestedResource;
}
