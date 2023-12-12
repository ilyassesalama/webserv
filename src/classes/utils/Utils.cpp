#include "../../../webserv.hpp"

Utils::WebservException::WebservException(const std::string &message) {
    this->message = (char*)message.c_str();
}

const char* Utils::WebservException::what() const throw() {
    return message;
}