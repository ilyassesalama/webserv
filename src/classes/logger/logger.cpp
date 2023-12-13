#include "../../../webserv.hpp"

bool Log::isInitialized = false;

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
    // clear log file
    if (!isInitialized) {
        std::ofstream logFile("webserverLogs.txt", std::ios::trunc);
        if (logFile.is_open()) {
            logFile.close();
            isInitialized = true;
        } else {
            std::cerr << "\033[1;31mLog file failed to open\n\033[0m" << std::endl;
            std::exit(1);
        }
    }
    // start logging activity
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);
    std::string finalMessage = "";
    std::ostringstream timeStream;
    timeStream << localTime->tm_year + 1900 << '-'
               << std::setw(2) << std::setfill('0') << localTime->tm_mon + 1 << '-'
               << std::setw(2) << std::setfill('0') << localTime->tm_mday << ' '
               << std::setw(2) << std::setfill('0') << localTime->tm_hour << ':'
               << std::setw(2) << std::setfill('0') << localTime->tm_min << ':'
               << std::setw(2) << std::setfill('0') << localTime->tm_sec;
    std::string timeString = timeStream.str();
    std::ofstream logFile("webserverLogs.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << timeString << "] " << message << std::endl;
        finalMessage = "[" + timeString + "] " + message;
        logFile.close();
        return finalMessage;
    } else {
        std::cerr << "\033[1;31mLog file failed to open\n\033[0m" << std::endl;
        std::exit(1);
    }
}

void setSocketNonBlocking(int socketFd) {
    int flag = fcntl(socketFd, F_GETFL, 0);

    if(flag == -1) {
        Log::e("fcntl eroor");
        close(socketFd);
        return;
    }
    if(fcntl(socketFd, F_SETFL, flag | O_NONBLOCK) == -1) {
        Log::e("fcntl eroor");
        close(socketFd);
    }
}