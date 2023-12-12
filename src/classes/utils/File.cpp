#include "../../../webserv.hpp"

std::string File::getFileContent(const std::string &path){
    std::ifstream file(path.c_str());
    if(!file.is_open()){
        throw Utils::WebservException("Can't open \"" + path + "\" due to " + std::string(strerror(errno)));
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

std::string File::getContentType(std::string path) {
    if (String::endsWith(path, ".css")) {
        return "text/css";
    } else if (String::endsWith(path, ".js")) {
        return "application/javascript";
    } else if (String::endsWith(path, ".html") || String::endsWith(path, ".htm")) {
        return "text/html";
    } else if (String::endsWith(path, ".jpg") || String::endsWith(path, ".jpeg")) {
        return "image/jpeg";
    } else if (String::endsWith(path, ".png")) {
        return "image/png";
    } else if (String::endsWith(path, ".gif")) {
        return "image/gif";
    } else if (String::endsWith(path, ".svg")) {
        return "image/svg+xml";
    } else if (String::endsWith(path, ".ico")) {
        return "image/x-icon";
    } else if (String::endsWith(path, ".ttf")) {
        return "font/ttf";
    } else {
        // default content type
        return "text/plain";
    }
}
