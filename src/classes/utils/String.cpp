#include "../../../webserv.hpp"

std::string String::getExtension(const std::string &path){
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos) {
		return "none";
	}
    return path.substr(pos);
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

size_t String::to_size_t(const std::string &str){
    std::stringstream ss(str);
    size_t num;
    ss >> num;
    return num;
}

std::string String::to_string(int num){
    std::stringstream ss;
    ss << num;
    return ss.str();
}