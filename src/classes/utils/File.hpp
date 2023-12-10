#pragma

#include "../../../webserv.hpp"

class File {
    public:
        static std::string getFileContent(const std::string &path);
        static std::string getExtension(const std::string &path);
        static bool isStaticResource(const std::string &file);
};

