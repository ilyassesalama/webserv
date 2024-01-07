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
    this->parsingState.headerLineOK = false;
    this->parsingState.headersOK = false;
    this->parsingState.bodyOK = false;
    this->parsingState.statusCode = 0;
    this->parsingState.statusMessage = "";
	this->chunkRemainder = 0;
	this->isFirstRequest = true;
	this->isCRLF = false;
	this->isRequestChunked = false;
	this->isRequestMultipart = false;
    this->route = NULL;
}

/*
    The first function to be called, specifically by ServerInstance::receiveRequest()
    to receive and merge the final full request.
*/
void RequestParser::mergeRequestChunks(std::string &requestInput) {
	this->requestData = requestInput;

	if(!parsingState.headerLineOK && (requestData.find("\r\n") != std::string::npos)) {
		parseRequestLine(requestData);
	}
	if(parsingState.headerLineOK && !parsingState.headersOK && (requestData.find("\r\n\r\n") != std::string::npos || requestData.find("\n\n") != std::string::npos)) {
		parseRequestHeaders(requestData);
	}
	if (this->isFirstRequest) {
    	verifyIfRequestIsSafe(); // can only be called once all the parsing is done
		if (this->parsingState.statusCode != 200) {
			this->parsingState.ok = true;
			return;
		}
	}
    if(parsingState.headersOK && !parsingState.bodyOK) {
        parseRequestBody(requestData);
        if (FULL_LOGGING_ENABLED) Log::v("Parsing request body finished with status: " + String::to_string(parsingState.bodyOK));
    }
	if (this->getRequestLine()["method"] == "GET" || this->getRequestLine()["method"] == "DELETE")
    	this->parsingState.ok = parsingState.headerLineOK && parsingState.headersOK; // don't care about the body since it's optional
	else
		this->parsingState.ok = parsingState.headerLineOK && parsingState.headersOK && parsingState.bodyOK; // don't care about the body since it's optional
    Log::v(std::string("Request has been received and parsed with ") +  (parsingState.ok ? "success" : "failure"));
    if (parsingState.ok && FULL_LOGGING_ENABLED) {
        logParsedRequest();
    }
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
    parsingState.headerLineOK = true;

    if(FULL_LOGGING_ENABLED) Log::v("Parsing request line finished with status: " + String::to_string(parsingState.headerLineOK));

    parseRequestParams(path);
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
    Handles only the request headers and set their values to their
    respective variables. (ex: Content-Type: text/html)
*/
void RequestParser::parseRequestHeaders(std::string &requestData) {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(requestData);
    while(std::getline(httpStream, line)){
        if(line.find("POST", 0) == 0 || line.find("GET", 0) == 0 || line.find("DELETE", 0) == 0){
            continue;
        }
        if(line == "\r") break; // reached the end of headers because of only \r (\n is gone because of getline)
        line = line.substr(0, line.find("\r"));
        std::string key = line.substr(0, line.find(":"));
        std::string value = line.substr(line.find(":") + 2);

        keyValuePairs[key] = value;
    }
    this->headers = keyValuePairs;
    parsingState.headersOK = true;

	this->isRequestChunked = Utils::isMapKeyExists(this->headers, "Transfer-Encoding") && this->headers["Transfer-Encoding"] == "chunked" ? true : false;
	this->isRequestMultipart = Utils::isMapKeyExists(this->headers, "Content-Type") && this->headers["Content-Type"].find("multipart/form-data") != std::string::npos ? true : false;

    if(FULL_LOGGING_ENABLED) Log::v("Parsing request headers finished with status: " + String::to_string(parsingState.headersOK));
}

/*
    Last check by the request parser, stores the body if it exists.
*/
void RequestParser::parseRequestBody(std::string &requestData){

    if (this->requestLine["method"] == "GET" || this->requestLine["method"] == "DELETE")
        return ;

	size_t found = requestData.find("\r\n\r\n") + 4;
	std::string requestBody = this->isFirstRequest ? requestData.substr(found) : requestData;

	if (requestBody.size() > this->server->client_body_size) {
		this->parsingState.statusCode = 413;
		this->parsingState.statusMessage = "Request Entity Too Large";
		this->parsingState.bodyOK = true;
		return;
	}

	if (!isRequestChunked && !isRequestMultipart) {
		if (this->isFirstRequest)
			this->fileName = File::getWorkingDir() + this->route->root + this->route->upload_path + File::generateFileName("uploaded") + File::getContentTypeExtension(this->headers["Content-Type"]);
		std::fstream myFile(this->fileName, std::ios::binary | std::ios::app);
		if (!myFile.is_open()) throw Utils::WebservException("Error opening file: " + this->fileName);
		myFile << requestBody;
		myFile.close();
	} else if (isRequestChunked) {

		if (this->isFirstRequest)
			this->fileName = File::getWorkingDir() + this->route->root + this->route->upload_path + File::generateFileName("uploaded") + File::getContentTypeExtension(this->headers["Content-Type"]);

		getChunkedData(requestBody);

	} else if (isRequestMultipart) {

		if (this->isFirstRequest) {
			getBoundary(this->headers["Content-Type"]);
			this->fileName = "/tmp/" + File::generateFileName("boundary");
		}
		getBoundaryContent(requestBody);
	}

	this->isFirstRequest = false;

	if (!isRequestChunked && !isRequestMultipart && File::getFileLength(this->parsingState, this->fileName) == String::to_size_t(getHeaders()["Content-Length"])) {
        parsingState.bodyOK = true;
    }
}

/*
    The last function to be called when the parsing has finished, this
    function makes sure that the request is safe so we can start sending the response.
*/
void RequestParser::verifyIfRequestIsSafe(){
	if (Utils::isMapKeyExists(this->headers, "Transfer-Encoding") && this->headers["Transfer-Encoding"] != "chunked") {
        this->parsingState.statusCode = 501;
        this->parsingState.statusMessage = "Not Implemented";
        return;
    }
    if (this->requestLine["method"] == "POST" && (!Utils::isMapKeyExists(this->headers, "Content-Length") && !this->isRequestChunked && !this->isRequestMultipart)){
		this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
    }
	if (this->isRequestChunked && this->isRequestMultipart) {
		this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
	}
    if (!isHeaderLineValid()){
        this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
    }
    if (this->requestLine["path"].length() > 2048){
        this->parsingState.statusCode = 414;
        this->parsingState.statusMessage = "URI Too Long";
        return;
    }
    if (!Utils::isMapKeyExists(this->headers, "Host") || this->headers["Host"].empty()) {
        this->parsingState.statusCode = 400;
        this->parsingState.statusMessage = "Bad Request";
        return;
    }
    if (!isPathAccessible()) {
        this->parsingState.statusCode = 404;
        this->parsingState.statusMessage = "Not Found";
        return;
    }
    if (!isMethodAllowed()){
        this->parsingState.statusCode = 405;
        this->parsingState.statusMessage = "Method Not Allowed";
        return;
    }
    if (isRedirection()) {
        this->parsingState.statusCode = 301;
        this->parsingState.statusMessage = "Moved Permanently";
        return;
    }
    if (File::isDirectory(this->requestResourcePath) && this->requestResourcePath[this->requestResourcePath.length() - 1] != '/'
			&& this->requestLine["method"] != "DELETE" && this->requestLine["method"] != "POST"){
        this->parsingState.statusCode = 301;
        this->parsingState.statusMessage = "Moved Permanently";
        return;
    }
	if (!parseContentType()) {
		this->parsingState.statusCode = 400;
		this->parsingState.statusMessage = "Bad Request";
		return;
	}
    this->parsingState.statusCode = 200;
}