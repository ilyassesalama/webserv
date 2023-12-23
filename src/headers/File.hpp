#pragma once

#include "../../webserv.hpp"

class File {
    public:
        static std::string getFileContent(const std::string &path);
        static std::string getContentType(std::string path);
        static bool isFile(const std::string &path);
        static bool isDirectory(const std::string &path);
		static std::string getWorkingDir();
        static size_t getFileSize(std::string path);
        static void deleteFile(std::string path);
        static std::string getCGIbinary(std::string path);
		static std::string generateFileName(std::string name);
};



