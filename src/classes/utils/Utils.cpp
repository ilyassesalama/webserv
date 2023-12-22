#include "../../../webserv.hpp"

bool Utils::isHeaderKeyExists(std::map<std::string, std::string> headers, std::string key){
    return headers.find(key) != headers.end();
}

char **Utils::mapToEnv(std::map<std::string, std::string> env_map){
    char **env = new char*[env_map.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = env_map.begin(); it != env_map.end(); it++){
        std::string env_var = it->first + "=" + it->second;
        env[i] = new char[env_var.length() + 1];
        strcpy(env[i], env_var.c_str());
        i++;
    }
    env[i] = NULL;
    return env;
}

Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}