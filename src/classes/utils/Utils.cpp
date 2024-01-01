#include "../../../webserv.hpp"

bool Utils::isMapKeyExists(std::map<std::string, std::string> map, std::string key){
    return map.find(key) != map.end();
}

char **Utils::convertMapToChar2D(std::map<std::string, std::string> map){
    char **arr = new char*[map.size() + 1];
    int i = 0;
    for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++){
        std::string key = it->first;
        std::string value = it->second;
        std::string arr_var = key + "=" + value;
        arr[i] = new char[arr_var.length() + 1];
        strcpy(arr[i], arr_var.c_str());
        i++;
    }
    arr[i] = NULL;
    return arr;
}

void Utils::freeArray(char **arr){
    int i = 0;
    while(arr[i] != NULL){
        delete[] arr[i];
        i++;
    }
    delete[] arr;
}

std::string Utils::generateRandomName() {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int nameLength = 10;
    std::string randomName;
    for (int i = 0; i < nameLength; ++i) {
        int randomIndex = std::rand() % characters.length();
        randomName += characters[randomIndex];
    }
    return randomName;
}

Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}