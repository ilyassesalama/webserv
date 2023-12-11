#pragma once

#include "../../webserv.hpp"

class Utils {


    public:
        class WebservException : public std::exception {
            private:
                char * message;
            public:
                WebservException(std::string message);
                virtual const char* what() const throw();
        };
};