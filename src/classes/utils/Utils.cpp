#include "../../../webserv.hpp"


Utils::WebservException::WebservException(const std::string& message) : message(message) {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}