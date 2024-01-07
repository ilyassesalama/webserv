#include "../../../webserv.hpp"

CGInstance::CGInstance(RequestParser &request) : request(request) {}

int timeout_occurred = 0;

void CGInstance::initCGInstance() {
    if(FULL_LOGGING_ENABLED) {
        Log::d("Initializing CGI instance...");
    }
    this->cgiFailureStatus = false;
    this->cgiStatusCode = "200";
    timeout_occurred = 0;
    setCGIPath(File::getCGIbinary(this->filePath, this->request.getRoute()));
    setCGIServer();
    setRequestBody();
    setEnvironnementVariables();
    executeScript();
}

void CGInstance::setRequestBody() {
    std::string requestBody = "";
    try {
        requestBody = File::getFileContent(request.getFileName());
    } catch (std::exception &e) {}
    this->cgiRequestBodySize = String::to_string(requestBody.size());
    this->cgiRequestBody = requestBody;
}

void CGInstance::setEnvironnementVariables() {
    std::string requestedLocation = request.getRequestLine()["path"]; // will return something like "/script.php"
    std::string scriptName = requestedLocation.substr(requestedLocation.rfind("/") + 1); // will return something like "script.php"
    std::string documentRoot = this->filePath.substr(0, this->filePath.rfind("/")); // will return where tf is the script folder

    std::map<std::string, std::string> env_map;
    env_map["REQUEST_METHOD"] = request.getRequestLine()["method"];
    env_map["QUERY_STRING"] = this->getQueryString(request.getParams());
    env_map["SCRIPT_FILENAME"] = this->filePath;
    env_map["SCRIPT_NAME"] = scriptName;
    env_map["DOCUMENT_ROOT"] = documentRoot;
    env_map["SERVER_NAME"] = this->request.getServerInformation()->listen.host;
    env_map["CONTENT_LENGTH"] = this->cgiRequestBodySize;
    env_map["CONTENT_TYPE"] = request.getHeaders()["Content-Type"];
    env_map["SERVER_PORT"] = String::to_string(this->request.getServerInformation()->listen.port);
    env_map["REDIRECT_STATUS"] = "1"; 
    for (std::map<std::string, std::string>::iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); it++) {
        std::string key = "HTTP_" + String::to_upper(it->first);
        std::string value = it->second;
        env_map[key] = value;
    }
    this->cgiEnv = Utils::convertMapToChar2D(env_map);

    if(FULL_LOGGING_ENABLED){
        Log::d("Generated CGI Environnement variables:");
        for (std::map<std::string, std::string>::iterator it = env_map.begin(); it != env_map.end(); it++) {
            std::cout << "- " << it->first << ": " << it->second << std::endl;
        }
    }
}

void execution_timeout(int sig) {
    (void)sig;
    timeout_occurred = 1;
}

void CGInstance::executeScript() {
    unsigned int max_execution_time = 5;

    if(FULL_LOGGING_ENABLED) Log::d("Everything is ready, now executing CGI script with max execution time of " + String::to_string(max_execution_time) + " seconds...");
    signal(SIGALRM, execution_timeout);
    alarm(max_execution_time); // set a timer

    FILE *file_in = tmpfile(), *file_out = tmpfile(); // create temporary files to store the request body and the response body
    int fd_in = fileno(file_in), fd_out = fileno(file_out); // get the file descriptors of the temporary files

    write(fd_in, this->cgiRequestBody.c_str(), String::to_size_t(this->cgiRequestBodySize)); // write the request body to the input fd
    lseek(fd_in, 0, SEEK_SET); // reset the file pointer to the beginning of the file

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);

        char *args[] = {(char *)this->cgiPath.c_str(), (char *)this->filePath.c_str(), NULL};

        execve(args[0], args, this->cgiEnv);
        throw Utils::WebservException("Cannot execute the script");

    } else if (pid == -1) {
        throw Utils::WebservException("Cannot fork a new child process to execute the script");
    } else {
        // parent process
        while (waitpid(pid, NULL, WNOHANG) == 0) {
            if (timeout_occurred) {
                kill(pid, SIGKILL); // kill the child process
                cgiFailureStatus = true;
                this->cgiStatusCode = "504";
                break;
            }
            usleep(1000); // sleep for a short time before trying again since we're not hanging the process anymore using WNOHANG
        }
        alarm(0); // cancel the alarm, don't show the error message
    }
    if (!timeout_occurred) {
        // alright the script has been executed successfully, now read the response body from the output fd from zero
        lseek(fd_out, 0, SEEK_SET);
        char buffer[1024];
        int readBytes = 1; 
        while (readBytes > 0) {
            readBytes = read(fd_out, buffer, sizeof(buffer) - 1);
            buffer[readBytes] = '\0';
            cgiResponse += buffer;
        }
    }
    Utils::freeArray(this->cgiEnv);
    fclose(file_in);
    fclose(file_out);
    close(fd_in);
    close(fd_out);

    parseResponseHeaders();
    parseResponseBody();
    printCGIResponse();
    saveSessionCookie();
}

void CGInstance::parseResponseHeaders() {
    if(this->cgiServer != "php") return;
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
    if(this->cgiResponse.find("\r\n\r\n") != std::string::npos)
        cgiResponse = cgiResponse.substr(cgiResponse.find("\r\n\r\n") + 4);
}

void CGInstance::setCGIServer() {
    if(String::endsWith(this->cgiPath, "php-cgi")) {
        this->cgiServer = "php";
    } else if (String::endsWith(this->cgiPath, "py-cgi")) {
        this->cgiServer = "python";
    }
}

void CGInstance::saveSessionCookie(){
    if(!Utils::isMapKeyExists(this->cgiResponseHeadersMap, "SessionID")) return;
    std::string cookie = this->cgiResponseHeadersMap["SessionID"];
    if(FULL_LOGGING_ENABLED) Log::w("Found a session cookie, saving it now...");
    std::ofstream cookieSessionsFile("sessions.txt", std::ios::app);
    if(!cookieSessionsFile.is_open()) {
        Log::e("Can't open sessions file");
        return;
    }
    cookieSessionsFile << "sessionId=" << cookie << std::endl;
	cookieSessionsFile.close();
}