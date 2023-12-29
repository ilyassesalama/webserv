#include "../../../webserv.hpp"

// getters

t_route* RequestParser::getRoute() {
    return(this->route);
}

std::string RequestParser::getRequestedResourcePath() {
    return(this->requestResourcePath);
}

std::map<std::string, std::string>& RequestParser::getRequestLine() {
    return this->requestLine;
}

std::map<std::string, std::string> &RequestParser::getHeaders(){
    return(this->headers);
}

std::map<std::string, std::string> const &RequestParser::getParams(){
    return(this->params);
}

std::string const &RequestParser::getBody(){
    return(this->body);
}

ParsingState const &RequestParser::getParsingState(){
    return(this->parsingState);
}

std::string &RequestParser::getRequestData() {
    return(requestData);
}

std::string &RequestParser::getFileName() {
    return(this->fileName);
}

// setters

void RequestParser::setServerInformation(t_server *server){
	this->server = server;
}

// helper functions

bool RequestParser::isPathAccessible() {
    this->requestResourcePath = "";
    size_t firstSlash = this->requestLine["path"].find_first_of("/");
    std::string location;
    bool slashRoute = false;

    if(firstSlash != std::string::npos) {
        size_t secondSlach = this->requestLine["path"].find_first_of("/", firstSlash + 1);
        if(secondSlach != std::string::npos) {
            location = this->requestLine["path"].substr(0, secondSlach);        
        }
        else location = "/";
    }
    t_route *route = NULL;
    t_route *slashR = NULL;
    for(std::vector<t_route>::iterator it = this->server->routes.begin(); it != this->server->routes.end(); ++it) {
        if(it->path == "/" && location != "/" && slashRoute == false) {
            slashRoute = true;
            slashR = &(*it);
        }
        if(location == it->path) {
            route = &(*it);
            break;
        }
    }
    if(route == NULL) {
        if(slashRoute == true) {
            this->route = slashR;
            this->requestResourcePath.append(File::getWorkingDir());
            this->requestResourcePath.append(slashR->root);
            this->requestResourcePath.append(getRequestLine()["path"]);
        }
    } else {
        this->route = route;
        this->requestResourcePath.append(File::getWorkingDir());
        this->requestResourcePath.append(route->root);
        this->requestResourcePath.append(getRequestLine()["path"]);
    }
    if(FULL_LOGGING_ENABLED)
        Log::v("RequestParser: Request resource path after merge: " + this->requestResourcePath);
    return(File::isFile(this->requestResourcePath) || File::isDirectory(this->requestResourcePath));
}

bool RequestParser::isMethodAllowed() {
    std::string method = getRequestLine()["method"];
    if(method != "GET" && method != "POST" && method != "DELETE")
        return(false);
    if(std::find(route->allowed_methods.begin(), route->allowed_methods.end(), method) == route->allowed_methods.end()){
        return(false);
    }
    return(true);
}

/*
    Check if the request line is valid. HTTP method is checked in isMethodAllowed().
    No need to check for it again.
*/
bool RequestParser::isHeaderLineValid(){
    std::string httpVersion = getRequestLine()["httpVersion"];
    std::string path = getRequestLine()["path"];

    if((this->requestLine["path"][0] != '/' || this->requestLine["path"].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._~:/?#[]@!$&'()*+,;=%-") != std::string::npos))
        return(false);
    if((this->requestLine["httpVersion"] != "HTTP/1.1"))
        return(false);
    return(true);
}

/*
    Adding a new allowed type to this list will require adding it
    in the File::getContentType() function as well.
*/
std::vector<std::string> getAllowedTypes() {
	std::vector<std::string>allowedTypes;

	allowedTypes.push_back("multipart/form-data");
	allowedTypes.push_back("text/html");
	allowedTypes.push_back("text/css");
	allowedTypes.push_back("application/javascript");
	allowedTypes.push_back("application/json");
	allowedTypes.push_back("image/jpeg");
	allowedTypes.push_back("image/png");
	allowedTypes.push_back("image/gif");
	allowedTypes.push_back("image/svg+xml");
	allowedTypes.push_back("image/x-icon");
	allowedTypes.push_back("font/ttf");
	allowedTypes.push_back("video/mp4");
	allowedTypes.push_back("application/octet-stream");
	allowedTypes.push_back("text/plain");
	allowedTypes.push_back("application/x-www-form-urlencoded");
	allowedTypes.push_back("application/pdf");
	allowedTypes.push_back("application/zip");
	allowedTypes.push_back("application/gzip");
	allowedTypes.push_back("audio/mp3");

	return allowedTypes;
}

bool checkMultiPartFormData(std::string contentType, std::string fullContentType, size_t paramPos) {
	if (contentType == "multipart/form-data") {
		skipSpaces(fullContentType, &paramPos);
		size_t startPos = fullContentType.find("boundary=");
		// error code bad request
		if (startPos == std::string::npos || startPos != paramPos)
			return false;
		std::string boundary = fullContentType.substr(startPos);
		size_t boundaryEndPos = boundary.find(" ");
		if (boundaryEndPos != std::string::npos) {
			skipSpaces(boundary, &boundaryEndPos);
			// error code bad request
			if (boundaryEndPos != boundary.size())
                return false;
		}
	}
	return true;
}

bool checkContentType(std::string fullContentType) {
	size_t endPos = fullContentType.find(";");
	if (endPos == std::string::npos) {
		endPos = fullContentType.size();
	}

	std::string contentType = fullContentType.substr(0, endPos);
	std::vector<std::string> allowedTypes = getAllowedTypes();
	if (std::find(allowedTypes.begin(), allowedTypes.end(), contentType) != allowedTypes.end()) {
		return checkMultiPartFormData(contentType, fullContentType, ++endPos);
	}
	return false;
}

bool RequestParser::parseContentType() {
    bool isExist = Utils::isMapKeyExists(this->headers, "Content-Type");

	if (isExist) {
		return checkContentType(this->headers["Content-Type"]);
	}
	this->headers["Content-Type"] = File::getContentType(this->requestLine["path"]);
	return true;
}

void RequestParser::logParsedRequest(){
    std::map<std::string, std::string>::iterator it;
    if (this->requestLine.empty()){
        Log::v("RequestParser: No request line found");
    } else {
        Log::v("RequestParser: Parsed request line:");
        for(it = this->requestLine.begin(); it != this->requestLine.end(); it++){
            std::cout << "- " << it->first << ": " << it->second << std::endl;
        }
    }
    std::map<std::string, std::string>::iterator it2;
    if (this->headers.empty()){
        Log::v("RequestParser: No headers found");
    } else {
        Log::v("RequestParser: Parsed headers:");
        for(it2 = this->headers.begin(); it2 != this->headers.end(); it2++){
            std::cout << "- " << it2->first << ": " << it2->second << std::endl;
        }
    }
    std::map<std::string, std::string>::iterator it3;
    if (this->params.empty()){
        Log::v("RequestParser: No params found");
    } else {
        Log::v("RequestParser: Parsed params:");
        for(it3 = this->params.begin(); it3 != this->params.end(); it3++){
            std::cout << "- " << it3->first << ": " << it3->second << std::endl;
        }
    }
    if (this->body.empty()){
        Log::v("RequestParser: No body found");
    } else {
        Log::v("RequestParser: Parsed body:");
        std::cout << this->body << "\n";
    }
    if(this->parsingState.statusCode != 0 && this->parsingState.statusCode != 200 && this->parsingState.statusCode != 201){
        Log::e("RequestParser: Parsing failed with code " + String::to_string(this->parsingState.statusCode) + " and reason: " + this->parsingState.statusMessage);
    }
}