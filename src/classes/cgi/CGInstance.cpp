#include "../../../webserv.hpp"

CGInstance::CGInstance(){

    REQUEST_METHOD = "REQUEST_METHOD=";
    QUERY_STRING = "QUERY_STRING="; 
    SCRIPT_FILENAME = "SCRIPT_FILENAME=";
    SCRIPT_NAME = "SCRIPT_NAME=";
    DOCUMENT_ROOT = "DOCUMENT_ROOT=";
    SERVER_NAME = "SERVER_NAME=0.0.0.0";
    SERVER_PORT = "SERVER_PORT=8080";
    REDIRECT_STATUS= "REDIRECT_STATUS=1";

}

void CGInstance::setPath(std::string path) {
    (*this).path = path;
}

void CGInstance::setcgiPath(std::string cgiPath) {
    (*this).cgiPath = cgiPath;
}

void CGInstance::setEnvironnementVariables(RequestParser *parser) {
    (*this).REQUEST_METHOD.append(parser->getRequestLine()["method"]);
    (*this).QUERY_STRING.append("");
    (*this).SCRIPT_FILENAME.append((*this).path);
    (*this).SCRIPT_NAME.append(path.substr(path.rfind("/")));
    (*this).DOCUMENT_ROOT.append(path.substr(0,path.rfind("/")));


    //need to be stores on a char double pointer 
}
