#include "../../../webserv.hpp"

const char *Utils::WebservException::what() const throw() {
	return message;
}