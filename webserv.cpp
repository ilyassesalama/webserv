#include "webserv.hpp"

void parsingTest(){
    RequestParser requestParser;
    std::map<std::string, std::string> requestLine;
    std::map<std::string, std::string> headers;
    std::string body;


    std::string httpRequest = "GET /index.html HTTP/1.1\r\n"
                              "Host: www.1337.ma\r\n"
                              "User-Agent: Mozilla/5.0\r\n"
                              "Accept: text/html,*/*\r\n"
                              "Accept-Language: en-us\r\n"
                              "Accept-Charset: ISO-8859-1,utf-8\r\n"
                              "Connection: keep-alive\r\n"
                              "\r\n"
                              "body of the request";

    requestParser.initRequestParser(httpRequest);

    requestLine = requestParser.getRequestLine();
    headers = requestParser.getHeaders();
    body = requestParser.getBody();

    std::cout << "-------------- request line ----------------" << std::endl;

    std::cout << "method: " << requestLine["method"] << std::endl;
    std::cout << "path: " << requestLine["path"] << std::endl;
    std::cout << "httpVersion: " << requestLine["httpVersion"] << std::endl;

    std::cout << "-------------- request headers ----------------" << std::endl;
    std::map<std::string, std::string>::iterator it;
    for (it = headers.begin(); it != headers.end(); it++){
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "--------------- body ---------------" << std::endl;

    std::cout << body << std::endl;

}

int main() {
    parsingTest();
} 