#include "../../../webserv.hpp"

Response::Response() {
    (*this).clientSidePath = File::getWorkingDir();
    (*this).path = "";
    (*this).server = NULL;
    (*this).request = NULL;
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
    (*this).bytesToRead = 1024;
    (*this).fileOffset = 0;
    (*this).bytesSent = 0;
    (*this).servingState = false;
}


Response::~Response() {}


void Response::feedDataToTheSender() {
    if(fileOffset == -1 && bytesSent == responseVector.size())
    {   
        //finished file : change the serving status so he can recv another requestes
        setServingState(false);
        return;
    }
    else if(bytesSent < responseVector.size()) {
        std::vector<char>tmp(responseVector.begin() + bytesSent,responseVector.end());
        (*this).responseVector = tmp;
    }
    else if(bytesSent == responseVector.size()) {
        bytesSent = 0;
        responseVector.clear();
        addDataToResponse((*this).readFileByOffset());
    }
}


void Response::responseBuilder() {
   try {
        //check if the parser failed
        if((*this).statusCode != 200 && (*this).statusCode != 201)
            throw(Utils::WebservException(String::to_string((*this).statusCode)));
        else {
            if (this->request->getRequestLine()["method"] == "GET") {
                GETResponseBuilder();
            }
        }
    }
    catch(Utils::WebservException &ex) {
        //build error Response based on the error code
        buildErrorResponse();
    }
} 


/*
	GETTER FUNCTIONS
*/

void Response::buildErrorResponse() {
    //get The Body Of the Error Page
    (*this).responseBody =  getErrorPageHTML();
    setResponseLine();
    setHeaders();
    (*this).response.append((*this).responseLine);
    (*this).response.append((*this).responseHeaders);
    (*this).response.append((*this).responseBody);
    addDataToResponse(response);
}

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

std::string Response::getErrorPageHTML(){
    std::string responseBody;
	std::string error_page;

    this->path = getErrorPagePath(this->server->error_pages, this->statusCode);
    try {
        responseBody = File::getFileContent(this->path);
    } catch (std::exception &e) {
        responseBody = "<html><body><h1>ERROR 403</h1><p>Forbidden</p></body></html>";
        Log::e("Error page not found, who edited the permissions MFs!");
    }
    return responseBody;
}



void Response::handleDirectoryRequest() {

    size_t slashPos = this->path.find_last_of("/");

    std::string indexHTML = slashPos != this->path.size() - 1 ? "/index.html" : "index.html";

    if (File::isFile(this->path + indexHTML)) {
		this->path += indexHTML;
        this->responseBody = File::getFileContent(this->path);
		this->statusCode = 200;
    } else {
        bool directory_listing = this->currentRoute->directory_listing;

        if (directory_listing) {
            // response should be list of files in the directory
			this->autoIndexHTMLBuilder(indexHTML);
        } else {
            this->statusCode = 403;
            this->responseBody = this->getErrorPageHTML();
        }
    }
}

std::string Response::readFileByOffset() {
    char buffer[(*this).bytesToRead];
    std::string content;
    std::ifstream file((*this).path.c_str(),std::ios::binary);

    if(!file.is_open()) {
        throw(Utils::WebservException("404"));
    }
    file.seekg((*this).fileOffset);
    file.read(buffer,(*this).bytesToRead);
    (*this).fileOffset = file.tellg();

    content.append(buffer,file.gcount());
    return(content);
}

void Response::handleFileRequest() {

    // std::vector<std::string>cgi_methods = this->currentRoute->cgi_methods;
    // std::string cgi_extension = this->currentRoute->cgi_extension;

    // if (cgi_methods.size() != 0) {

    //     if (!cgi_extension.empty() && std::find(cgi_methods.begin(), cgi_methods.end(), "GET") != cgi_methods.end()) {

    //     }

    // }
    // else 
        this->responseBody = readFileByOffset();

}

void Response::clearResponse() {
    (*this).path = "";
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
	(*this).clientSidePath = File::getWorkingDir();
    (*this).responseVector.clear();
    (*this).fileOffset = 0;
    (*this).servingState = false;
    (*this).bytesSent = 0;
}

void Response::buildResourcePath(t_route *route) {
    std::string requestedResource;

    requestedResource.append(this->clientSidePath);
    requestedResource.append(route->root);
    requestedResource.append(this->path);
    (*this).path = requestedResource;
}


