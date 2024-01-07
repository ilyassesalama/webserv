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

std::string File::getContentTypeExtension(const std::string &type) {
    if (type == "text/css") {
        return ".css";
    } else if(type == "video/mp4") {
        return ".mp4";
    } else if(type == "application/pdf") {
        return ".pdf";
    } else if (type == "application/javascript" || type == "text/javascript") {
        return ".js";
    } else if (type == "text/html") {
        return ".html";
    } else if (type == "image/jpeg") {
        return ".jpeg";
    } else if (type == "image/png") {
        return ".png";
    } else if (type == "image/gif") {
        return ".gif";
    } else if (type == "image/svg+xml") {
        return ".svg";
    } else if (type == "image/x-icon") {
        return ".ico";
    } else if (type == "font/ttf") {
        return ".ttf";
    } else {
        return "";  // default
    }
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
    } else if(String::endsWith(path, ".pdf")) {
        return("application/pdf");
    } else if (String::endsWith(path, ".js")) {
        return "application/javascript";
    } else if (String::endsWith(path, ".html") || String::endsWith(path, ".htm") || String::endsWith(path, ".php") || String::endsWith(path, ".py")) {
        return "text/html charset=UTF-8";
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

std::string File::getCGIbinary(std::string path, t_route *route) {
	std::string cgiBinPath = getCurrentDir() + "/src/classes/cgi/bin";
    if(String::endsWith(path, ".php") && std::find(route->cgi_extension.begin(), route->cgi_extension.end(), "php") != route->cgi_extension.end()) {
        return cgiBinPath.append("/php-cgi");
    } else if (String::endsWith(path, ".py") && std::find(route->cgi_extension.begin(), route->cgi_extension.end(), "py") != route->cgi_extension.end()) {
        return cgiBinPath.append("/py-cgi");
    } else {
        return "";
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
	return getCurrentDir() + "/src/client-side";
}

size_t File::getFileSize(std::string path) {
    std::ifstream file(path.c_str());

    if(!file.is_open()) {
        Log::e("getFileSize: Fallback to 0 because can't get the size of \"" + path + "\" due to " + std::string(strerror(errno)));
        return 0;
    }
    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg();
    file.close();
    if(size == -1){
        Log::e("getFileSize: Fallback to 0 because can't get the size of \"" + path + "\" due to " + std::string(strerror(errno)));
        return 0;
    }
    return static_cast<size_t>(size);
}

std::string File::generateFileName(std::string name) {
	timeval currentTime;
    gettimeofday(&currentTime, 0);
	long long microseconds = static_cast<long long>(currentTime.tv_sec) * 1000000LL + currentTime.tv_usec;
	std::string number;
	std::stringstream strstream;
	strstream << microseconds;
	strstream >> number;
	std::string fileName = "file_" + number + "_" + name;
	return fileName;
}

std::string File::getCurrentDir() {
    char cwd[PATH_MAX];
    std::string path = "";
    getcwd(cwd, sizeof(cwd));
    path.append(cwd);
    return path;
}

bool File::deleteLocation(std::string path) {
    return(File::isFile(path) && unlink(path.c_str()) == 0);
}

size_t File::getFileLength(ParsingState &parsingState, std::string fileName) {
	std::fstream myFile(fileName, std::ios::binary | std::ios::app);
	if (!myFile.is_open()) {
		parsingState.ok = true;
		parsingState.statusCode = 500;
		throw Utils::WebservException("Error opening file : " + fileName);
	}
	myFile.seekg(0, std::ios::end);
    std::size_t length = myFile.tellg();
    myFile.seekg(0, std::ios::beg);
	myFile.close();
	return length;
}