#include "../../../webserv.hpp"

RequestParser::RequestParser(){
    nullOutVars();
}

void RequestParser::nullOutVars(){
    // null out the request line, headers and body to avoid segfaults >:(
    this->requestLine = std::map<std::string, std::string>();
    this->headers = std::map<std::string, std::string>();
    this->body = "";
    this->parsingState.ok = false;
    this->parsingState.headLineOk = false;
    this->parsingState.headsOk = false;
    this->parsingState.bodyOk = false;
    this->parsingState.failCode = 0;
    this->parsingState.failReason = "";
}

void RequestParser::mergeRequestChunks(std::string &requestInput) {
    this->requestData.append(requestInput);

    if(!parsingState.headLineOk && (requestData.find("\r\n") != std::string::npos)) {
        parseRequestLine(requestData);
        if(FULL_LOGGING_ENABLED) Log::v("Parsing request line finished with status: " + String::to_string(parsingState.headLineOk));
    }
    if(parsingState.headLineOk && !parsingState.headsOk && (requestData.find("\r\n\r\n") != std::string::npos || requestData.find("\n\n") != std::string::npos)) {
        parseRequestHeaders(requestData);
        if(FULL_LOGGING_ENABLED) Log::v("Parsing request headers finished with status: " + String::to_string(parsingState.headsOk));
    }
    if(parsingState.headsOk && !parsingState.bodyOk) {
        parseRequestBody(requestData);
        if(FULL_LOGGING_ENABLED) Log::v("Parsing request body finished with status: " + String::to_string(parsingState.bodyOk));
    }
    parsingState.ok = parsingState.headLineOk && parsingState.headsOk; // don't care about the body since it's optional
    Log::d("Request parsing finished with status: " + String::to_string(parsingState.ok));
    if (parsingState.ok && FULL_LOGGING_ENABLED) {
        parseFinalRequest();
        logParsedRequest();
    }
}

void RequestParser::parseFinalRequest(){
    if(!this->headers["Transfer-Encoding"].empty() && this->headers["Transfer-Encoding"] != "chunked"){
        this->parsingState.failCode = 501;
        this->parsingState.failReason = "Not Implemented";
        return;
    }
    if(this->requestLine["method"] == "POST" && (!Utils::isHeaderKeyExists(this->headers, "Content-Length") || this->headers["Transfer-Encoding"].empty())){
        this->parsingState.failCode = 400;
        this->parsingState.failReason = "Bad Request";
        return;
    }
    if(this->requestLine["path"].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._~:/?#[]@!$&'()*+,;=%") != std::string::npos){
        this->parsingState.failCode = 400;
        this->parsingState.failReason = "Bad Request";
        return;
    }
    if(this->requestLine["path"].length() > 2048){
        this->parsingState.failCode = 414;
        this->parsingState.failReason = "URI Too Long";
        return;
    }
    size_t bodyMaxSizeFromConfig = (this->server->client_body_size * 1000000);
    if(this->body.length() > bodyMaxSizeFromConfig){
        this->parsingState.failCode = 413;
        this->parsingState.failReason = "Request Entity Too Large";
        return;
    }
    if (!isPathAccessible()) {
        this->parsingState.failCode = 404;
        this->parsingState.failReason = "Not Found";
        return;
    }
    if(!isMethodAllowed()){
        this->parsingState.failCode = 405;
        this->parsingState.failReason = "Method Not Allowed";
        return;
    }
	if (!parseContentType()) {
		this->parsingState.failCode = 400;
		this->parsingState.failReason = "Bad Request";
		std::cout << "ENTERED" << std::endl;
	}

}

void RequestParser::parseRequestLine(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    // only get the first line
    if(std::getline(httpStream, line))
    ;
    std::string method = line.substr(0, line.find(" "));
    if(method != "GET" && method != "POST" && method != "DELETE"){
        throw(Utils::WebservException("Error: Invalid HTTP method\n"));
    }
    std::string path = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1);
    if(path[0] != '/'){
        throw(Utils::WebservException("Error: Invalid HTTP path\n"));
    }
    std::string httpVersion = line.substr(line.rfind(" ") + 1);
    if (httpVersion.find("HTTP/") == std::string::npos){
        throw(Utils::WebservException("Error: Invalid HTTP version\n"));
    }
    keyValuePairs["method"] = method;
    keyValuePairs["path"] = path;
    keyValuePairs["httpVersion"] = httpVersion;
    this->requestLine = keyValuePairs;
    parsingState.headLineOk = true;
    parseRequestParams(path);
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
    parsingState.headsOk = true;
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

void RequestParser::parseRequestBody(std::string &requestData){
    size_t found = requestData.rfind("\r\n\r\n");
    this->body += requestData.substr(found + 4);
    if(requestData.length() - found - 4 == String::to_size_t(getHeaders()["Content-Length"])) {
        parsingState.bodyOk = true;
    }
}
