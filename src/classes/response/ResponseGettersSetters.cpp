#include "../../../webserv.hpp"



/*
	GETTER FUNCTIONS
*/




std::vector<char>& Response::getResponse() {
	return((*this).responseVector);
}



std::string Response::getStringStatus(){
    switch(this->statusCode){
        case 200:
            return "200 OK";
		case 301:
            return "301 Moved Permanently";
        case 400:
            return "400 Bad Request";
        case 404:
            return "404 Not Found";
        case 403:
            return "403 Forbidden";
        case 405:
            return "405 Method Not Allowed";
        case 409:
            return "409 Conflict";
        case 413:
            return "413 Request Entity Too Large";
        case 414:
            return "414 Request-URI Too Long";
        case 501:
            return "501 Not Implemented";
        default:
            return "500 Internal Server Error";
    }
}

std::string getPathLocation(std::string path) {
    std::string location;

    size_t firstSlash = path.find("/");

    if(firstSlash != std::string::npos) {
        size_t secondSlash = path.find("/", firstSlash + 1);
        if(secondSlash != std::string::npos) {
             return path.substr(0, secondSlash); 
        }
    }
    return("/");
}



/*
	SETTER FUNCTIONS
*/



void Response::setPath(std::string requestedResourcePath) {
    (*this).path = requestedResourcePath;
}

void Response::setStatusCode(int code) {
    (*this).statusCode = code;
}

void Response::setRequest(RequestParser &request) {
    (*this).request = &request;
}

void Response::setServer(t_server &server) {
    (*this).server = &server;
}

void Response::setResponseLine() {

    std::string status = getStringStatus();

    this->responseLine = "HTTP/1.1 " + status + "\r\n";
}

std::string addHeaders(std::string key, std::string value) {
    std::stringstream header;

    header << key;
    header << ": ";
    header << value;
    header << "\r\n";

    return(header.str());
}

void Response::setHeaders() {

    (*this).responseHeaders.append(addHeaders("Content-Type", File::getContentType(this->path)));
    (*this).responseHeaders.append(addHeaders("Content-Length", String::to_string(File::getFileSize((*this).path))));
    //add more headers based on the status code
    if(statusCode != 200 && statusCode != 201) {
        (*this).responseHeaders.append(addHeaders("Connection", "close"));
    }
    if(statusCode == 301) {
        (*this).responseHeaders.append(addHeaders("Location", this->request->getRequestLine()["path"] + "/"));
    }
    (*this).responseHeaders.append("\r\n");
}

void Response::setResponseBody() {
	if (File::isDirectory(this->path)) {
        this->handleDirectoryRequest();
    } 
    else if (File::isFile(this->path)) {
        this->handleFileRequest();
    }
}

void Response::setRoute(t_route *route) {
    (*this).currentRoute = route;

}


void Response::addDataToResponse(std::string data) {
    responseVector.insert(responseVector.end(), data.begin(), data.end());
}

bool Response::isServing() {
    return((*this).servingState);
}

void Response::setServingState(bool status) {
    (*this).servingState = status;
}
void Response::setBytesSent(size_t bytes) {
    (*this).bytesSent = bytes;
}