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
    this->parsingState.failCode = 0;
    this->parsingState.failReason = "";
	this->chunkRemainder = 0;
	this->isChunked = false;
}

/*
    The first function to be called, specifically by ServerInstance::recvRequest()
    to receive and merge the final full request.
*/
void RequestParser::mergeRequestChunks(std::string &requestInput) {
	if (this->headers.empty())
    	this->requestData.append(requestInput);
	else if (Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding"))
		this->requestData = requestInput;

	if(!parsingState.headLineOk && (requestData.find("\r\n") != std::string::npos)) {
		parseRequestLine(requestData);
		if(FULL_LOGGING_ENABLED) Log::v("Parsing request line finished with status: " + String::to_string(parsingState.headLineOk));
	}
	if(parsingState.headLineOk && !parsingState.headsOk && (requestData.find("\r\n\r\n") != std::string::npos || requestData.find("\n\n") != std::string::npos)) {
		std::cout << "MAXBODY" << std::endl;
		parseRequestHeaders(requestData);
		if(FULL_LOGGING_ENABLED) Log::v("Parsing request headers finished with status: " + String::to_string(parsingState.headsOk));
	}
    if(parsingState.headsOk && !parsingState.bodyOk) {
        parseRequestBody(requestData);
        if(FULL_LOGGING_ENABLED) Log::v("Parsing request body finished with status: " + String::to_string(parsingState.bodyOk));
    }
	if (!this->isChunked) {
    	verifyIfRequestIsSafe(); // can only be called once all the parsing is done
		this->isChunked = Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding") ? true : false;
	}
	if (this->getRequestLine()["method"] == "GET" || this->getRequestLine()["method"] == "DELETE")
    	this->parsingState.ok = parsingState.headLineOk && parsingState.headsOk; // don't care about the body since it's optional
	else
    	this->parsingState.ok = parsingState.headLineOk && parsingState.headsOk && parsingState.bodyOk; // don't care about the body since it's optional
    Log::d("Request parsing finished with status: " + String::to_string(parsingState.ok));
    if (parsingState.ok && FULL_LOGGING_ENABLED) {
		std::cout << "parsing state ok" << std::endl;
        logParsedRequest();
    }
}

/*
    The last function to be called when the parsing has finished, this
    function makes sure that the request is safe so we can start sending the response.
*/
void RequestParser::verifyIfRequestIsSafe(){
	if (Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding") && this->headers["Transfer-Encoding"] != "chunked") {
        this->parsingState.failCode = 501;
        this->parsingState.failReason = "Not Implemented";
        return;
    }
    if(this->requestLine["method"] == "POST" && (!Utils::isHeaderKeyExists(this->headers, "Content-Length") || !Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding"))){
        this->parsingState.failCode = 400;
        this->parsingState.failReason = "Bad Request";
        return;
    }
    if(!isHeaderLineValid()){
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
    if(File::isDirectory(this->requestResourcePath) && this->requestResourcePath[this->requestResourcePath.length() - 1] != '/'){
        this->parsingState.failCode = 301;
        this->parsingState.failReason = "Moved Permanently";
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
		return;
	}
    this->parsingState.failCode = 200;
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
}

/*
    NOT NECESSARY, but will probably be useful later?
    Handles only the URI parameters and set their values to their
    respective variables. (ex: ?key=value&key2=value2)
*/
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

/*
	returns remaining chunk size of previous request
	if chunk size is not less than request size it returns request size
*/

size_t getChunkEnd(std::string body, size_t chunkRemainder) {

	size_t i;
	for (i = 0; i < body.size(); i++) {
		if (chunkRemainder == 0)
			return i;
		chunkRemainder--;
	}
	return i;
}

/*
	returns chunk end because there may be two chunks in one request or this request ends with 0
*/

size_t getChunkedRequestBodyEnd(std::string &body, size_t chunkRemainder) {

	size_t endPos = body.rfind("\r\n0\r\n\r\n");
	if (endPos == std::string::npos) {
		endPos = body.rfind("\n0\n\n");
		if (endPos != std::string::npos && endPos == body.size() - 4)
			return endPos;
		else
			return getChunkEnd(body, chunkRemainder);
	} else {
		if (endPos != std::string::npos && endPos == body.size() - 7)
			return endPos;
	}
	return getChunkEnd(body, chunkRemainder);
}

/*
	this function subtracts the size of the next chunk from the request body
	the main function only calls this when chunk remainder is zero
*/

size_t getChunkSize(std::string body) {

	size_t pos = body.find("\r\n");
	if (pos == std::string::npos)
		pos = body.find("\n");

	std::string firstLine = body.substr(0, pos);


	size_t hexSize;
	std::stringstream buffer(firstLine);
	buffer >> std::hex >> hexSize;

	return hexSize;
}

/*
	this function checks whether the request contains 0 which means end of request
*/

size_t getZero(std::string &body) {
	size_t zeroPos = body.rfind("\r\n0\r\n\r\n");
	if (zeroPos == std::string::npos) {
		zeroPos = body.rfind("\n0\n\n");
		if (zeroPos != std::string::npos && zeroPos == body.size() - 4)
			return zeroPos;
		else
			return std::string::npos;
	} else {
		if (zeroPos != std::string::npos && zeroPos == body.size() - 7)
			return zeroPos;
		else
			return std::string::npos;
	}
	return zeroPos;
}

/*
	this is the main function to get chunks and it works using recursion
*/

void RequestParser::getChunkedData(std::string &body) {

	// try {
		if (this->chunkRemainder == 0) {
		this->chunkRemainder = getChunkSize(body);
		size_t pos = body.find("\r\n"); // removing chunk size from body
		if (pos == std::string::npos) {
			pos = body.find("\n");
			pos += 1;
		} else {
			pos += 2;
		}
		body = body.substr(pos);
	}

	std::fstream myFile(File::getWorkingDir() + "/tmp1.mp4", std::ios::binary | std::ios::app);
	if (!myFile.is_open()) {
		this->parsingState.ok = false;
		return ;
	}

	bool isZero = getZero(body) == std::string::npos ? false : true;

	size_t bodyEnd = getChunkedRequestBodyEnd(body, this->chunkRemainder);

	std::string chunkStr = body.substr(0, bodyEnd);

	myFile << chunkStr;

	this->chunkRemainder -= chunkStr.size();

	body = body.substr(bodyEnd);

	myFile.close();

	if (isZero == true && this->chunkRemainder == 0) {
		std::cout << "parsing state ok my function" << std::endl;
		this->parsingState.bodyOk = true;
		this->parsingState.failCode = 201;
		return ;
	}

	if (body.size() != 0) {

		size_t pos = body.find("\r\n"); // removing chunk size from body
		if (pos == std::string::npos) {
			pos = body.find("\n");
			pos += 1;
		} else {
			pos += 2;
		}

		body = body.substr(pos);

		if (body.size() != 0)
			getChunkedData(body);
		else
			return ;
	}
	// } catch (...) {
	// 	std::cerr << "CATCHED!" << std::endl;
	// 	exit(1);
	// }
}

/*
    Last check by the request parser, stores the body if it exists.
*/
void RequestParser::parseRequestBody(std::string &requestData){
    size_t found = requestData.rfind("\r\n\r\n");

	this->body += requestData.substr(found + 4);
	if (!this->isChunked) {

		if (Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding")) {
			std::string firstChunk = requestData.substr(found + 4);

			getChunkedData(firstChunk);
		}
	} else if (Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding"))
		getChunkedData(requestData);
    else if(!Utils::isHeaderKeyExists(this->headers, "Transfer-Encoding") && requestData.length() - found - 4 == String::to_size_t(getHeaders()["Content-Length"])) {
		std::cout << "parsing state ok salama function" << std::endl;
        parsingState.bodyOk = true;
    }
}
