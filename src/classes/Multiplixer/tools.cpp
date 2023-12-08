#include"tools.hpp"

void Log::v(std::string message) {
    std::cout << "\033[1;35m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::d(std::string message) {
    std::cout << "\033[3;34m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::i(std::string message) {
    std::cout << "\033[3;79m" << formatLoggingMessage(message) << "\033[0m" << std::endl; 
}

void Log::w(std::string message) {
    std::cout << "\033[3;33m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::e(std::string message) {
    std::cerr << "\033[3;31m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

std::string Log::formatLoggingMessage(std::string message){
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);
    std::string finalMessage = "";

    char timeString[100];
    // WARNING: `strftime` is part of C++11 not C++98
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

    std::ofstream logFile ("webserverLogs.txt" , std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << timeString << "] " << message << std::endl;
        finalMessage = "[" + std::string(timeString) + "] " + message;
        logFile.close();
        return(finalMessage);
    } else {
        std::cerr << "\033[1;31mLog file failed to open\n\033[0m" << std::endl;
        std::exit(1);
    }
}