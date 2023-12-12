#pragma once

#include "../../webserv.hpp"

class Utils {
    public:
        class WebservException : public std::exception {
            private:
                char *message;
            public:
                WebservException(const std::string& message);
                const char* what() const throw();
            };
};