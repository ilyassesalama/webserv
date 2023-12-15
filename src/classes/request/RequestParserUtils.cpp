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