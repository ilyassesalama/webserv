#include "../../../webserv.hpp"

Response::Response() {


    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    (*this).clientSidePath.append(cwd);
    (*this).clientSidePath.append("/src/client-side");
    (*this).path = "";
    (*this).server = NULL;
    (*this).request = NULL;
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
}

Response::~Response() {

}

/*
	GETTER FUNCTIONS
*/

std::string Response::getAllowedMethod(std::string location, std::string method) {
    for(std::vector<t_route>::iterator it = (*this).routes.begin(); it != (*this).routes.end(); ++it) {
        if(location == it->path) {
            if(std::find(it->allowed_methods.begin(), it->allowed_methods.end(), method) == it->allowed_methods.end())
                return("METHOD NOT ALLOWED IN THIS LOCATION");
            else return(method);
        }
    }
    return("NO LOCATION FOUND");
}

std::string Response::getResponse() {
	return((*this).response);
}

std::string Response::getErrorPageHTML(){

    std::string responseBody;

    switch(this->statusCode){
		case 301:
            this->path = "src/client-side/error_pages/301.html";
            responseBody = File::getFileContent(this->path);
			break;
        case 400:
            this->path = "src/client-side/error_pages/400.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 404:
            this->path = "src/client-side/error_pages/404.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 403:
            this->path = "src/client-side/error_pages/403.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 405:
            this->path = "src/client-side/error_pages/405.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 409:
            this->path = "src/client-side/error_pages/409.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 413:
            this->path = "src/client-side/error_pages/413.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 414:
            this->path = "src/client-side/error_pages/414.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 500:
            this->path = "src/client-side/error_pages/500.html";
            responseBody = File::getFileContent(this->path);
            break;
        default:
            this->path = "src/client-side/error_pages/501.html";
            responseBody = File::getFileContent(this->path);
            break;
    }
    return responseBody;
}


std::string Response::getStringStatus(){
    switch(this->statusCode){
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
        case 500:
            return "500 Internal Server Error";
        default:
            return "501 Not Implemented";
    }
}

t_route *Response::getSpecificRoute(std::string location) {
    for(std::vector<t_route>::iterator it = (*this).routes.begin(); it != (*this).routes.end(); ++it) {
        if(location == it->path) {
            return(&(*it));
        }
    }
    return(NULL);
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


void Response::setStatusCode(int code) {
    (*this).statusCode = code;
}

void Response::setPath(std::string path,std::string method) {
    (*this).path = path;
    std::string locationPath = getPathLocation(path);

	this->currentRoute = getSpecificRoute(locationPath);

    if(this->currentRoute == NULL) {
		//resource not found set error code 
		setStatusCode(404);
    }
    else  {
        //check if the method allowed on that location 
        if(getAllowedMethod(locationPath,method) == method)
            buildResourcePath(this->currentRoute);
        else
            setStatusCode(405);
    }
}

void Response::setRequest(RequestParser &request) {
    (*this).request = &request;
}

void Response::setServer(t_server &server) {
    (*this).server = &server;
    setRoute();
}

void Response::setResponseLine() {

    std::string status = getStringStatus();

    this->responseLine = "HTTP/1.1 " + status;
}

void Response::setHeaders() {

	std::string contentType;
	std::string contentSize;

	contentType = File::getContentType(this->path);
	(*this).contentLength = this->responseBody.size();
	contentSize = String::to_string((*this).contentLength);
	this->responseHeaders = "\nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n";
}

void Response::setResponseBody() {

	std::vector<std::string> allowed_methods;
	bool uploadSupport;

	if (this->currentRoute != NULL) {
		allowed_methods = this->currentRoute->allowed_methods;
		uploadSupport = true;
	}

	if (this->statusCode != 200 && this->statusCode != 0) {
		this->responseBody = this->getErrorPageHTML();
		return ;
	}

	if (allowed_methods.size() != 0 && std::find(allowed_methods.begin(), allowed_methods.end(), "POST") != allowed_methods.end() && this->request->getRequestLine()["method"] == "POST" && uploadSupport) {
		
	} else if (File::isDirectory(this->path)) {
        this->handleDirectoryRequest();
    } else if (File::isFile(this->path)) {
        this->handleFileRequest();
    } else {
        this->statusCode = 404;
        this->responseBody = this->getErrorPageHTML();
    }
}

void Response::setRoute() {
    (*this).routes = server->routes;
}

/*
	Other member functions
*/

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

void Response::handleFileRequest() {

    std::vector<std::string>cgi_methods = this->currentRoute->cgi_methods;
    std::string cgi_extension = this->currentRoute->cgi_extension;

    if (cgi_methods.size() != 0) {

        if (!cgi_extension.empty() && std::find(cgi_methods.begin(), cgi_methods.end(), "GET") != cgi_methods.end()) {

        }

    } else {
        this->responseBody = File::getFileContent(this->path);
		this->statusCode = 200;
    }

}

void Response::buildResourcePath(t_route *route) {
    std::string requestedResource;

    requestedResource.append(this->clientSidePath);
    requestedResource.append(route->root);
    requestedResource.append(this->path);
    (*this).path = requestedResource;
}

void Response::responseBuilder() {
    if (this->request->getRequestLine()["method"] == "GET") {
        this->setPath(request->getRequestLine()["path"],"GET");
		Log::e(path);
        this->GETResponseBuilder();
        
    } else if (this->request->getRequestLine()["method"] == "POST") {
		this->setPath(request->getRequestLine()["path"],"POST");
		// Log::w(path);
		this->POSTResponseBuilder();
    } else if (this->request->getRequestLine()["method"] == "DELETE") {

    }
}

void Response::clearResponse() {
    (*this).path = "";
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
    char cwd[PATH_MAX];
    (*this).clientSidePath.clear();
    getcwd(cwd, sizeof(cwd));
    (*this).clientSidePath.append(cwd);
    (*this).clientSidePath.append("/src/client-side");
}