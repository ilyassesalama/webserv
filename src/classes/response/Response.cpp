#include "../../../webserv.hpp"

Response::Response(){
    this->response = "";
    this->status = 500;
}

Response::Response(const std::string &path){
    Log::i("Generating response from \"" + path + "\"...");
    std::ifstream file(path);
    if(!file.is_open()){
        this->status = 404;
        Log::e("Failed to open response file, skipping...");
    }
    std::string line;
    while(std::getline(file, line)){
        this->response += line;
    }
    if(FULL_LOGGING_ENABLED) Log::d("Response: " + this->response);
    file.close();
    this->response += "\r\n";
    this->status = 200;
}

void Response::sendResponse(int &clientFD) {
    std::string reponseHTML = this->response;
    std::string responseLen = std::to_string(reponseHTML.length());

    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + responseLen + "\n\n" + reponseHTML;
    send(clientFD, response.c_str(), response.length(), 0);
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