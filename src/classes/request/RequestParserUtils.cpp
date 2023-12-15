#include "../../../webserv.hpp"

// getters

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

// setters

void RequestParser::setServerInformation(t_server *server){
	this->server = server;
}

// helper functions

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
    if(this->parsingState.failCode != 0){
        Log::v("RequestParser: Parsing failed with code " + String::to_string(this->parsingState.failCode) + " and reason: " + this->parsingState.failReason);
    }
}

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

            this->requestResourcePath.append(File::getWorkingDir());
            this->requestResourcePath.append(slashR->root);
            this->requestResourcePath.append(getRequestLine()["path"]);
        }
    } else {
        this->requestResourcePath.append(File::getWorkingDir());
        this->requestResourcePath.append(route->root);
        this->requestResourcePath.append(getRequestLine()["path"]);
    }
    Log::w("RequestParser: Request resource path: " + this->requestResourcePath);
    return(File::isFile(this->requestResourcePath) || File::isDirectory(this->requestResourcePath));
}

bool RequestParser::isMethodAllowed() {
    size_t firstSlash = this->requestLine["path"].find_first_of("/");
    std::string location;

    if(firstSlash != std::string::npos) {
        size_t secondSlach = this->requestLine["path"].find_first_of("/", firstSlash + 1);
        if(secondSlach != std::string::npos) {
            location = this->requestLine["path"].substr(0, secondSlach);        
        }
        else location = "/";
    }
    for(std::vector<t_route>::iterator it = this->server->routes.begin(); it != this->server->routes.end(); ++it) {
        if(location == it->path) {
            if(std::find(it->allowed_methods.begin(), it->allowed_methods.end(), getRequestLine()["method"]) == it->allowed_methods.end())
                return(false);
            else return(true);
        }
    }
    return(false);
}