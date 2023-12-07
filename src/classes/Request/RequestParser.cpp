#include "../../headers/RequestParser.hpp"

RequestParser::RequestParser(){
    // null out the request line, headers and body to avoid segfaults >:(
    this->requestLine = std::map<std::string, std::string>();
    this->headers = std::map<std::string, std::string>();
    this->body = "";
}

void RequestParser::initRequestParser(std::string &requestData){
    parseRequestLine(requestData);
    parseRequestHeaders(requestData); // this will also parse the body
}

void RequestParser::parseRequestLine(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    while(std::getline(httpStream, line)){
        if(line.rfind("POST", 0) == 0 || line.rfind("GET", 0) == 0 || line.rfind("DELETE", 0) == 0){
            break;
        }
    }
    std::string method = line.substr(0, line.find(" "));
    if(method != "GET" && method != "POST" && method != "DELETE"){
        std::cout << "Error: Invalid HTTP method" << std::endl;
        exit(1);
    }
    std::string path = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1);
    if(path[0] != '/'){
        std::cout << "Error: Invalid path" << std::endl;
        exit(1);
    }
    std::string httpVersion = line.substr(line.rfind(" ") + 1);
    if (httpVersion.find("HTTP/") == std::string::npos){
        std::cout << "Error: Invalid HTTP version" << std::endl;
        exit(1);
    }
    keyValuePairs["method"] = method;
    keyValuePairs["path"] = path;
    keyValuePairs["httpVersion"] = httpVersion;

    this->requestLine = keyValuePairs;
}

void RequestParser::parseRequestHeaders(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    while(std::getline(httpStream, line)){
        if(line.rfind("POST", 0) == 0 || line.rfind("GET", 0) == 0 || line.rfind("DELETE", 0) == 0){
            continue;
        }
        if(line == "\r") break; // reached the end of headers
        std::string key = line.substr(0, line.find(":"));
        std::string value = line.substr(line.find(":") + 2);
        keyValuePairs[key] = value;
    }
    this->headers = keyValuePairs;
    parseRequestBody(httpStream);
}

void RequestParser::parseRequestBody(std::stringstream &httpStream) {
    // i saved the body line by line, idk if it's the best way to do it but ok lol
    std::string line;
    std::string body = "";
    while(std::getline(httpStream, line)){
        body += line;
    }
    this->body = body;
}

std::map<std::string, std::string> const &RequestParser::getRequestLine(){
    return(this->requestLine);
}

std::map<std::string, std::string> const &RequestParser::getHeaders(){
    return(this->headers);
}

std::string const &RequestParser::getBody(){
    return(this->body);
}