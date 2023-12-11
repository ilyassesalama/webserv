#pragma once

#include "../../webserv.hpp"

class String {
    public:
        static std::string getExtension(const std::string &path);
        static bool endsWith(const std::string &str, const std::string &suffix);
};