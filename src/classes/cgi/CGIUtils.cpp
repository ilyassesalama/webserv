#include "../../../webserv.hpp"

void CGInstance::setFilePath(std::string filePath) {
    this->filePath = filePath;
}

void CGInstance::setCGIPath(std::string cgiPath) {
    this->cgiPath = cgiPath;
}

std::string CGInstance::getCGIResponse() {
    return this->cgiResponse;
}

bool CGInstance::hasCGIFailed() {
    return this->cgiFailureStatus;
}

std::string CGInstance::getCGIContentType() {
    if(!Utils::isMapKeyExists(this->cgiResponseHeadersMap, "Content-Type")) return "text/html charset=UTF-8";
    return this->cgiResponseHeadersMap["Content-Type"];
}

int CGInstance::getCGIStatusCode() {
    if(!Utils::isMapKeyExists(this->cgiResponseHeadersMap, "Status")){
        Log::w("CGI response does not contain a status code " + cgiStatusCode);
        return String::to_int(cgiStatusCode);
    }
    Log::w("CGI response contains a status code " + this->cgiResponseHeadersMap["Status"]);
    return String::to_int(this->cgiResponseHeadersMap["Status"]);
}

size_t CGInstance::getCGIContentLength() {
    size_t contentLength = 0;
    if(Utils::isMapKeyExists(this->cgiResponseHeadersMap, "Content-Length")) {
        contentLength = String::to_size_t(this->cgiResponseHeadersMap["Content-Length"]);
    } else {
        contentLength = this->cgiResponse.size();
    }
    return contentLength;
}

std::string CGInstance::getQueryString(std::map<std::string, std::string> queries) {
    std::string queryString = "";
    for (std::map<std::string, std::string>::iterator it = queries.begin(); it != queries.end(); it++) {
        queryString += it->first + "=" + it->second + "&";
    }
    queryString = queryString.substr(0, queryString.size() - 1);
    return queryString;
}

void CGInstance::printCGIResponse() {
    if(!FULL_LOGGING_ENABLED) return;
    Log::v("CGI Response: Parsed headers:");
    for (std::map<std::string, std::string>::iterator it = this->cgiResponseHeadersMap.begin(); it != this->cgiResponseHeadersMap.end(); it++) {
        std::cout << "- " << it->first << ": " << it->second << std::endl;
    }
    Log::v("Body: ");
    std::cout << this->cgiResponse << std::endl;
}