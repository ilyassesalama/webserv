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
        static std::string getCGIbinary(std::string path, t_route *route);
		static std::string generateFileName(std::string name);
        static std::string getCurrentDir();
		static std::string getContentTypeExtension(const std::string &type);
        static bool deleteLocation(std::string path);
        static size_t getFileLength(ParsingState &parsingState, std::string fileName);
};