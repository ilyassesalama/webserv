#include"tools.hpp"

void Log::d(std::string message) {
    std::cout << "\033[1;35m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::i(std::string message) {
    std::cout << "\033[1;34m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::w(std::string message) {
    std::cout << "\033[1;33m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::e(std::string message) {
    std::cout << "\033[1;31m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

std::string Log::formatLoggingMessage(std::string message){
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);
    std::string finalMessage = "";

    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

    std::ofstream logFile ("webserverLogs.txt" , std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << timeString << "] " << message << std::endl;
        finalMessage = "[" + std::string(timeString) + "] " + message;
        logFile.close();
        return(finalMessage);
    } else {
        std::cerr << "Log file not opened" << std::endl;
        std::exit(1);
    }
}