#include "../../../webserv.hpp"

std::string String::getExtension(const std::string &path){
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos) {
		return "none";
	}
    return path.substr(pos);
}

bool String::endsWith(const std::string &str, const std::string &suffix){
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
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

bool String::is_a_num(std::string str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) {
            throw(Utils::WebservException("Is Not A Number"));
        }
    }
    return true;
}

bool String::isIpFormCorrect(std::string ip) {
    int dotCount = 0;
	while(ip.find(".") != std::string::npos) {
		size_t dotPosition = ip.find(".");
        if(dotPosition != std::string::npos)
            dotCount++;
		try {
            String::is_a_num(ip.substr(0,dotPosition));
		}
		catch(...) {
			return(false);
		}
        ip = ip.substr(dotPosition + 1);
	}
    if(dotCount == 3) {
        try {
            String::is_a_num(ip);
        }
        catch(...) {
            return(false);
        }
    } 
    else
        return false;
    return(true);
}

// bool String::isIpFormCorrect(std::string ip) {
//     std::istringstream ss(ip);
//     std::string segment;
//     int dotCount = 0;

//     while (std::getline(ss, segment, '.')) {
//         if (++dotCount > 3) {
//             return false; // Too many segments
//         }

//         if (!is_a_num(segment)) {
//             return false;
//         }
//     }

//     return dotCount == 3;
// }