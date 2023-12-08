#include "../../../webserv.hpp"

RequestParser::RequestParser(){
    nullOutVars();
}

RequestParser::RequestParser(const std::string &requestData){
    nullOutVars();
    initRequestParser(const_cast<std::string &>(requestData));
}

void RequestParser::nullOutVars(){
     // null out the request line, headers and body to avoid segfaults >:(
    this->requestLine = std::map<std::string, std::string>();
    this->headers = std::map<std::string, std::string>();
    this->body = "";
}

void RequestParser::initRequestParser(std::string &requestData){
    setParsingState(REQ_PARSER_STARTED);
    try {
        parseRequestLine(requestData); // this will also parse the url params
        parseRequestHeaders(requestData); // this will also parse the body
        setParsingState(REQ_PARSER_OK);
        if(FULL_LOGGING_ENABLED){
            Log::d("RequestParser: Request parsing completed successfully");
            logParsedRequest();
        }
    } catch (const RequestParser::RequestParserException &e) {
        Log::e(e.what());
        setParsingState(REQ_PARSER_FAILED);
        exit(1);
    }
}

void RequestParser::parseRequestLine(std::string &requestData) {
    setParsingState(REQ_PARSER_HEAD_LINE_PENDING);
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
        throw("Error: Invalid HTTP method\n");
    }
    std::string path = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1);
    if(path[0] != '/'){
        throw("Error: Invalid HTTP path\n");
    }
    std::string httpVersion = line.substr(line.rfind(" ") + 1);
    if (httpVersion.find("HTTP/") == std::string::npos){
        throw("Error: Invalid HTTP version\n");
    }
    keyValuePairs["method"] = method;
    keyValuePairs["path"] = path;
    keyValuePairs["httpVersion"] = httpVersion;
    this->requestLine = keyValuePairs;
    setParsingState(REQ_PARSER_HEAD_LINE_OK);
    parseRequestParams(path);
}

void RequestParser::parseRequestHeaders(std::string &requestData) {
    setParsingState(REQ_PARSER_HEADS_PENDING);
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
    setParsingState(REQ_PARSER_HEADS_OK);
    parseRequestBody(httpStream);
}

void RequestParser::parseRequestParams(std::string &requestData){
    size_t start = requestData.find("?"); // look for the first param
    if (start == std::string::npos) return; // no parameters found! get out!!!

    std::map<std::string, std::string> keyValuePairs;
    std::string token;
    std::istringstream tokenStream(requestData.substr(start + 1)); // start from the first param

    while (std::getline(tokenStream, token, '&')) { // look for each param separated by &
        size_t equalPos = token.find('=');
        if (equalPos != std::string::npos) {
            std::string key = token.substr(0, equalPos);
            std::string value = token.substr(equalPos + 1);
            this->params[key] = value;
        }
    }
}

void RequestParser::parseRequestBody(std::stringstream &httpStream) {
    setParsingState(REQ_PARSER_BODY_PENDING);
    // i saved the body line by line, idk if it's the best way to do it but ok lol
    std::string line;
    std::string body = "";
    while(std::getline(httpStream, line)){
        body += line;
    }
    setParsingState(REQ_PARSER_BODY_OK);
    this->body = body;
}

std::map<std::string, std::string> const &RequestParser::getRequestLine(){
    return(this->requestLine);
}

std::map<std::string, std::string> const &RequestParser::getHeaders(){
    return(this->headers);
}

std::map<std::string, std::string> const &RequestParser::getParams(){
    return(this->params);
}

std::string const &RequestParser::getBody(){
    return(this->body);
}

PrasingState const RequestParser::getParsingState(){
    return(this->parsingState);
}

void RequestParser::setParsingState(PrasingState state){
    this->parsingState = state;
}

const char *RequestParser::RequestParserException::what() const throw() {
	return this->message;
}





// helper functions

void RequestParser::logParsedRequest(){
    std::map<std::string, std::string>::iterator it;
    Log::d("RequestParser: Parsed request:");
    for(it = this->requestLine.begin(); it != this->requestLine.end(); it++){
        std::cout << it->first << ": " << it->second << std::endl;
    }
    Log::d("RequestParser: Parsed headers:");
    for(it = this->headers.begin(); it != this->headers.end(); it++){
        std::cout << it->first << ": " << it->second << std::endl;
    }
    Log::d("RequestParser: Parsed params:");
    for(it = this->params.begin(); it != this->params.end(); it++){
        std::cout << it->first << ": " << it->second << std::endl;
    }
    Log::d("RequestParser: Parsed body:");
    std::cout << this->body << std::endl;
}