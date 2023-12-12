#pragma once

#include "../../webserv.hpp"

class Utils {
public:
    class WebservException : public std::exception {
    private:
        std::string message;

    public:
        WebservException(const std::string& message) throw();
        virtual ~WebservException() throw(); 
        const char* what() const throw();
    };
};

