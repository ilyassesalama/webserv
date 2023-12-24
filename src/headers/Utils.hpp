#pragma once

#include "../../webserv.hpp"

class Utils {
    public:
        static bool isMapKeyExists(std::map<std::string, std::string> headers, std::string key);
        static char **mapToEnv(std::map<std::string, std::string> env_map);
        static void freeArray(char **arr);

        class WebservException : public std::exception {
        private:
            std::string message;

        public:
            WebservException(const std::string& message) throw();
            virtual ~WebservException() throw(); 
            const char* what() const throw();
        };
};

