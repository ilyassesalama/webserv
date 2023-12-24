#include "../../../webserv.hpp"

bool Utils::isMapKeyExists(std::map<std::string, std::string> headers, std::string key){
    return headers.find(key) != headers.end();
}

char **Utils::mapToEnv(std::map<std::string, std::string> env_map){
    char **env = new char*[env_map.size() + 1];
    int i = 0;
    for(std::map<std::string, std::string>::iterator it = env_map.begin(); it != env_map.end(); it++){
        std::string key = it->first;
        std::string value = it->second;
        std::string env_var = key + "=" + value;
        env[i] = new char[env_var.length() + 1];
        strcpy(env[i], env_var.c_str());
        i++;
    }
    env[i] = NULL;
    return env;
}

void Utils::freeArray(char **arr){
    int i = 0;
    while(arr[i] != NULL){
        delete[] arr[i];
        i++;
    }
    delete[] arr;
}

Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}