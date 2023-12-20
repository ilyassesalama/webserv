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

/*
    Adding a new allowed type to this list will require adding it
    to the list of allowed types in the std::vector<std::string> getAllowedTypes() method as well.
*/
std::string File::getContentType(std::string path) {
    if (String::endsWith(path, ".css")) {
        return "text/css";
    } else if(String::endsWith(path,".mp4")) {
        return("video/mp4");
    }else if(String::endsWith(path, ".pdf")) {
        return("application/pdf");
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
        return "text/plain";  // default content type
    }
}

std::string File::getCGIbinary(std::string path) {
    if(String::endsWith(path,".php")) {
        return "php-cgi";
    } else if (String::endsWith(path,".php")) {
        return "py-cgi";
    } else {
        return "WALO" ; // :3 
    }
}

bool File::isFile(const std::string& path) {
    if(access(path.c_str(), F_OK) == -1)
        return false;
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISREG(statbuf.st_mode);
}

bool File::isDirectory(const std::string& path) {
    if(access(path.c_str(), F_OK) == -1)
        return false;
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

std::string File::getWorkingDir(){
	char cwd[PATH_MAX];
	std::string path = "";
    getcwd(cwd, sizeof(cwd));
    path.append(cwd).append("/src/client-side");
	return path;
}

size_t File::getFileSize(std::string path) {
    std::ifstream file(path.c_str());

    if(!file.is_open()) {
        throw(Utils::WebservException("getFileSize: Can't open \"" + path + "\" due to " + std::string(strerror(errno))));
    }
    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg();
    file.close();
    if(size == -1)
        throw(Utils::WebservException("getFileSize: Can't get size of \"" + path + "\" due to " + std::string(strerror(errno))));
    return static_cast<size_t>(size);
}

void File::deleteFile(std::string path) {
    if (remove(path.c_str()) != 0) {
        throw(Utils::WebservException("Can't delete \"" + path + "\" due to " + std::string(strerror(errno))));
    }
}