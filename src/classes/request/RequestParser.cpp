#include "../../../webserv.hpp"

RequestParser::RequestParser(){
    nullOutVars();
}

/*
    Null out all the variables in the class to avoid any server failures.
*/
void RequestParser::nullOutVars(){
    this->requestLine = std::map<std::string, std::string>();
    this->headers = std::map<std::string, std::string>();
    this->body = "";
    this->parsingState.ok = false;
    this->parsingState.headLineOk = false;
    this->parsingState.headsOk = false;
    this->parsingState.bodyOk = false;
    this->parsingState.statusCode = 0;
    this->parsingState.statusMessage = "";
	this->chunkRemainder = 0;
	this->isFirstRequest = true;
	this->isCRLF = false;
	this->isRequestChunked = false;
	this->isRequestMultipart = false;
}

/*
    The first function to be called, specifically by ServerInstance::receiveRequest()
    to receive and merge the final full request.
*/
void RequestParser::mergeRequestChunks(std::string &requestInput) {

	if (this->headers.empty() || (!this->isRequestChunked && !this->isRequestMultipart))
    	this->requestData.append(requestInput);
	else if (this->isRequestChunked || this->isRequestMultipart)
		this->requestData = requestInput;

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
	if (this->isFirstRequest) {
    	verifyIfRequestIsSafe(); // can only be called once all the parsing is done
		this->isFirstRequest = false;
	}
	if (this->getRequestLine()["method"] == "GET" || this->getRequestLine()["method"] == "DELETE")
    	this->parsingState.ok = parsingState.headLineOk && parsingState.headsOk; // don't care about the body since it's optional
	else
		this->parsingState.ok = parsingState.headLineOk && parsingState.headsOk && parsingState.bodyOk; // don't care about the body since it's optional
    Log::d("Request parsing finished with status: " + String::to_string(parsingState.ok));
    if (parsingState.ok && FULL_LOGGING_ENABLED) {
        logParsedRequest();
    }
}

/*
    The last function to be called when the parsing has finished, this
    function makes sure that the request is safe so we can start sending the response.
*/
void RequestParser::verifyIfRequestIsSafe(){

	if (!this->isRequestChunked) {
        this->parsingState.statusCode = 501;
        this->parsingState.statusMessage = "Not Implemented";
        return;
    }
    if(this->requestLine["method"] == "POST" && (!Utils::isMapKeyExists(this->headers, "Content-Length") && !this->isRequestChunked && !this->isRequestMultipart)){
		this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
    }
    if(!isHeaderLineValid()){
        this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
    }
    if(this->requestLine["path"].length() > 2048){
        this->parsingState.statusCode = 414;
        this->parsingState.statusMessage = "URI Too Long";
        return;
    }
    size_t bodyMaxSizeFromConfig = this->server->client_body_size;
    if(this->body.length() > bodyMaxSizeFromConfig){
        this->parsingState.statusCode = 413;
        this->parsingState.statusMessage = "Request Entity Too Large";
        return;
    }
    if (!isPathAccessible()) {
        this->parsingState.statusCode = 404;
        this->parsingState.statusMessage = "Not Found";
        return;
    }
    if(File::isDirectory(this->requestResourcePath) && this->requestResourcePath[this->requestResourcePath.length() - 1] != '/'
			&& this->requestLine["method"] != "DELETE" && this->requestLine["method"] != "POST"){
        this->parsingState.statusCode = 301;
        this->parsingState.statusMessage = "Moved Permanently";
        return;
    }
    if(!isMethodAllowed()){
        this->parsingState.statusCode = 405;
        this->parsingState.statusMessage = "Method Not Allowed";
        return;
    }
	if (!parseContentType()) {
		this->parsingState.statusCode = 400;
		this->parsingState.statusMessage = "Bad Request";
		return;
	}
    this->parsingState.statusCode = 200;
}

/*
    Handles only the request line and set their values to their
    respective variables. (ex: GET /index.html HTTP/1.1)
*/
void RequestParser::parseRequestLine(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    // only get the first line
    if(std::getline(httpStream, line))
    ;
    line = line.substr(0, line.find("\r"));
    std::string method = line.substr(0, line.find(" "));
    std::string path = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1);
    std::string httpVersion = line.substr(line.rfind(" ") + 1);
    keyValuePairs["method"] = method;
    keyValuePairs["path"] = path;
    keyValuePairs["httpVersion"] = httpVersion;

    this->requestLine = keyValuePairs;
    parsingState.headLineOk = true;
    parseRequestParams(path);
}

/*
    Handles only the request headers and set their values to their
    respective variables. (ex: Content-Type: text/html)
*/
void RequestParser::parseRequestHeaders(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    while(std::getline(httpStream, line)){
        if(line.rfind("POST", 0) == 0 || line.rfind("GET", 0) == 0 || line.rfind("DELETE", 0) == 0){
            continue;
        }
        if(line == "\r") break; // reached the end of headers
        line = line.substr(0, line.find("\r"));
        std::string key = line.substr(0, line.find(":"));
        std::string value = line.substr(line.find(":") + 2);

        keyValuePairs[key] = value;
    }
    this->headers = keyValuePairs;
    parsingState.headsOk = true;

	this->isRequestChunked = Utils::isMapKeyExists(this->headers, "Transfer-Encoding") && this->headers["Transfer-Encoding"] == "chunked" ? true : false;
	this->isRequestMultipart = Utils::isMapKeyExists(this->headers, "Content-Type") && this->headers["Content-Type"].find("multipart/form-data") != std::string::npos ? true : false;
}

/*
    Handles only the URI parameters and set their values to their
    respective variables. (ex: ?key=value&key2=value2)
    Will be used in the cgi part.
*/
void RequestParser::parseRequestParams(std::string &requestData){
    size_t start = requestData.find("?"); // look for the first param
    if (start == std::string::npos) return; // no parameters found! get out!!!

	// clear all after '?'
	requestLine["path"] = requestLine["path"].substr(0, start);
	
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

/*
    Last check by the request parser, stores the body if it exists.
*/
void RequestParser::parseRequestBody(std::string &requestData){
	size_t found = requestData.find("\r\n\r\n") + 4;

	std::string requestBody = this->isFirstRequest ? requestData.substr(found) : requestData;

	if (!isRequestChunked) {
		this->body = requestData.substr(found);
	} else if (isRequestChunked) {

		if (this->isFirstRequest)
			this->fileName = File::generateFileName("uploaded") + File::getExtension(headers);

		getChunkedData(requestBody);

	}
	
	if (isRequestMultipart) {

		if (this->isFirstRequest) {
			getBoundary(this->headers["Content-Type"]);
			this->fileName = File::generateFileName("boundary");
		}
		getBoundaryContent(requestBody);
	}

	if(!isRequestChunked && !isRequestMultipart && this->body.length() == String::to_size_t(getHeaders()["Content-Length"])) {
		
        parsingState.bodyOk = true;
    }
}
