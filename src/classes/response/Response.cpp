#include "../../../webserv.hpp"

Response::Response(int clientFd, const std::string &path){
    this->clientFD = clientFd;
    std::string finalPath = "src/client-side/main";
    if(path == "/")
        finalPath += "/index.html";
    else
        finalPath += path;

    Log::i("Fetching response from \"" + finalPath + "\"...");
    std::string content = File::getFileContent(finalPath);
    if(content == "\r"){
        this->status = 404;
        Log::e("Response: 404 Not Found");
        return;
    }
    this->response = content;
    this->status = 200;
}

void Response::sendResponse() {
    if(this->status != 200){
        this->getErrorPageHTML();
    }
    std::string reponse = this->response;
    std::string responseSize = std::to_string(reponse.length());
    std::string status = getStringStatus();
    Log::d("Sending response: " + response);

    response = "HTTP/1.1 " + status + " \nContent-Type: text/html\nContent-Length: " + responseSize + "\n\n" + reponse;
    send(clientFD, response.c_str(), response.length(), 0);
    Log::i("Response sent");
}

std::string Response::getErrorPageHTML(){
    switch(status){
        case 404:
            this->response = File::getFileContent("src/client-side/error_pages/404.html");
            break;
        case 500:
            this->response = File::getFileContent("src/client-side/error_pages/500.html");
            break;
        default:
            this->response = File::getFileContent("src/client-side/error_pages/501.html");
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