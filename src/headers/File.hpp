#pragma once

#include "libs.hpp"
#include "String.hpp"
#include "logger.hpp"

class File {
    public:
        static std::string getFileContent(const std::string &path);
        static std::string getContentType(std::string path);
};

