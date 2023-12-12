#include "../../../webserv.hpp"


Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}