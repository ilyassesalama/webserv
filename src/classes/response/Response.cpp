#include "../../../webserv.hpp"

Response::Response(int clientFd, const RequestParser &parser){
    this->clientFD = clientFd;
    this->requestParser = parser;
    std::string path = requestParser.getRequestLine()["path"];
    std::string finalPath = "src/client-side";
    if(path == "/")
        finalPath += "/main/index.html";
    else
        finalPath += path;

    Log::i("Fetching response from \"" + finalPath + "\"...");
    std::string content = File::getFileContent(finalPath);
    this->path = finalPath;
    if(content == "\0"){
        this->status = 404;
        Log::e("Response: 404 Not Found");
        return;
    }
    this->response = content;
    this->status = 200;
}

void Response::sendResponse() {
    if(this->status != 200){ // TOFIX: THIS WILL MAKE OTHER FILES SUCH AS CSS AND JS TO BE SENT AS HTML. FIX THIS!
        this->getErrorPageHTML();
    }
    this->contentType = File::getContentType(path);
    this->contentSize = std::to_string(this->response.length());
    std::string status = getStringStatus();

    response = "HTTP/1.1 " + status + " \nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n" + response;
    send(clientFD, response.c_str(), response.length(), 0);
    Log::i("Response sent");
    Log::d("Response information:\n- Content-Type: " + contentType + "\n- Content-Length: " + contentSize + "\n- Status: " + status + "\n- Path: " + path);
}

std::string Response::getErrorPageHTML(){
    switch(status){
        case 404:
            this->path = "src/client-side/error_pages/404.html";
            this->response = File::getFileContent(this->path);
            break;
        case 500:
            this->path = "src/client-side/error_pages/500.html";
            this->response = File::getFileContent(this->path);
            break;
        default:
            this->path = "src/client-side/error_pages/501.html";
            this->response = File::getFileContent(this->path);
            break;
    }
    return this->response;
}

void Response::setPath(const std::string &path){
    this->path = path;
}

const std::string &Response::getResponse(){
    return this->response;
}

const int &Response::getStatus(){
    return this->status;
}

std::string Response::getStringStatus(){
    switch(this->status){
        case 200:
            return "200 OK";
        case 404:
            return "404 Not Found";
        case 500:
            return "500 Internal Server Error";
        default:
            return "501 Unknown";
    }
}