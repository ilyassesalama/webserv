#include "../../../webserv.hpp"

CGInstance::CGInstance(RequestParser &request) : request(request) {
}

void CGInstance::setEnvironnementVariables() {
    std::string requestedLocation = request.getRequestLine()["path"]; // will return something like "/script.php"
    std::string scriptName = requestedLocation.substr(requestedLocation.rfind("/") + 1); // will return something like "script.php"
    std::string documentRoot = this->filePath.substr(0, this->filePath.rfind("/")); // will return where tf is the script folder

    std::map<std::string, std::string> env_map;
    env_map["REQUEST_METHOD"] = request.getRequestLine()["method"];
    env_map["QUERY_STRING"] = "";
    env_map["SCRIPT_FILENAME"] = this->filePath;
    env_map["SCRIPT_NAME"] = scriptName;
    env_map["DOCUMENT_ROOT"] = documentRoot;
    env_map["SERVER_NAME"] = "localhost";
    env_map["SERVER_PORT"] = "8080";
    env_map["REDIRECT_STATUS"] = "1";
    this->cgiEnv = Utils::mapToEnv(env_map);
}

void CGInstance::executeScript() {
    int readBytes;
    FILE *file_in = tmpfile(), *file_out = tmpfile();
    int fd_in = fileno(file_in), fd_out = fileno(file_out);

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        if (execve(this->cgiPath.c_str(), NULL, this->cgiEnv) == -1) {
            throw Utils::WebservException("Cannot execute the script");
        }
    } else if (pid == -1) {
        // Fork failed
        throw Utils::WebservException("Cannot fork a new child process to execute the script");
    } else {
        // Parent process
        char buffer[1024];
        waitpid(pid, NULL, 0);
        lseek(fd_out, 0, SEEK_SET);
        
        while ((readBytes = read(fd_out, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[readBytes] = '\0';
            cgiResponse += buffer;
        }
    }

    fclose(file_in);
    fclose(file_out);
    close(fd_in);
    close(fd_out);

    parseResponseHeaders();
    parseResponseBody();
    printCGIResponse();
}

void CGInstance::parseResponseHeaders() {
    std::map<std::string, std::string> keyValuePairs;
    std::string line;

    std::stringstream httpStream(this->cgiResponse);
    while(std::getline(httpStream, line)){
        if(line == "\r") break; // reached the end of headers
        line = line.substr(0, line.find("\r"));
        std::string key = line.substr(0, line.find(":"));
        std::string value = line.substr(line.find(":") + 2);

        keyValuePairs[key] = value;
    }
    this->cgiResponseHeadersMap = keyValuePairs;
}

void CGInstance::parseResponseBody() {
    cgiResponse = cgiResponse.substr(cgiResponse.find("\r\n\r\n") + 4);
}

void CGInstance::printCGIResponse() {
    Log::v("CGI Response: Parsed headers:");
    for (std::map<std::string, std::string>::iterator it = this->cgiResponseHeadersMap.begin(); it != this->cgiResponseHeadersMap.end(); it++) {
        std::cout << "- " << it->first << ": " << it->second << std::endl;
    }
    Log::v("Body: ");
    std::cout << this->cgiResponse << std::endl;
}

void CGInstance::setFilePath(std::string filePath) {
    this->filePath = filePath;
}

void CGInstance::setCGIPath(std::string cgiPath) {
    this->cgiPath = cgiPath;
}

std::string CGInstance::getCGIResponse() {
    return this->cgiResponse;
}

std::string CGInstance::getCGIContentType() {
    return this->cgiResponseHeadersMap["Content-Type"];
}

int CGInstance::getCGIStatusCode() {
    if(!Utils::isHeaderKeyExists(this->cgiResponseHeadersMap, "Status")) return 200;
    return String::to_int(this->cgiResponseHeadersMap["Status"]);
}