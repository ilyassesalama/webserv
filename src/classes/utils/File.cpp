#include "../../../webserv.hpp"

std::string File::getFileContent(const std::string &path){
    std::ifstream file(path);
    if(!file.is_open()){
        Log::e("getFileContent: Can't open \"" + path + "\" due to " + std::string(strerror(errno)));
        return "\r";
    }
    std::string line, content;
    while(std::getline(file, line)) {
        content += line;
        if (!file.eof())
            content += "\n";
    }
    file.close();
    return content;
}

std::string File::getExtension(const std::string &path){
    std::string extension = path.substr(path.find_last_of(".") + 1);
    return extension;
}

bool File::isStaticResource(const std::string &file){
    std::string extension = getExtension(file);
    if(extension != "html")
        return true;
    return false;
}