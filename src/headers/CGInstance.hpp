#pragma once

#include "../../webserv.hpp"

class CGInstance {
    public:
        CGInstance();
        ~CGInstance();

        void setEnvironnementVariables();
        void executeScript();
    private:
        std::string path; // going to be constructed with
        std::string cgiPath;
};