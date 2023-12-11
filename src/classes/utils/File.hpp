#pragma once

#include "../../../webserv.hpp"

class File {
    public:
        static std::string getFileContent(const std::string &path);
        static std::string getContentType(std::string path);
};

