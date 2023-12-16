#include "../../../webserv.hpp"


Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}

bool Utils::isHeaderKeyExists(std::map<std::string, std::string> headers, std::string key){
    return headers.find(key) != headers.end();
}