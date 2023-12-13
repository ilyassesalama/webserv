#include "../../../webserv.hpp"

std::string String::getExtension(const std::string &path){
    std::string extension = path.substr(path.find_last_of("."));
    return extension;
}

bool String::endsWith(const std::string &str, const std::string &suffix){
    return getExtension(str) == suffix;
}

int String::to_int(const std::string &str){
    std::stringstream ss(str);
    int num;
    ss >> num;
    return num;
}