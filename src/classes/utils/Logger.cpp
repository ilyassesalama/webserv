#include "../../../webserv.hpp"

bool Log::isInitialized = false;
char Log::logFilter = NO_FILTER;

void Log::v(std::string message) {
    if(!(logFilter == NO_FILTER || logFilter == VERBOSE)) return;
    std::cout << "\033[3;35m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::d(std::string message) {
    if(!(logFilter == NO_FILTER || logFilter == DEBUG)) return;
    std::cout << "\033[3;34m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::i(std::string message) {
    if(!(logFilter == NO_FILTER || logFilter == INFO)) return;
    std::cout << "\033[3;79m" << formatLoggingMessage(message) << "\033[0m" << std::endl; 
}

void Log::w(std::string message) {
    if(!(logFilter == NO_FILTER || logFilter == WARNING)) return;
    std::cout << "\033[3;33m" << formatLoggingMessage(message) << "\033[0m" << std::endl;
}

void Log::e(std::string message) {
    if(!(logFilter == NO_FILTER || logFilter == ERROR)) return;
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
            std::cerr << "\033[1;31mCan't open log file, logging in file will be disabled!\n\033[0m" << std::endl;
        }
    }
    // start logging activity
    std::string finalMessage = "";
    std::ofstream logFile("webserverLogs.txt", std::ios::app);
    finalMessage = "[+] " + message;
    if (logFile.is_open()) {
        logFile << finalMessage << std::endl;
        logFile.close();
    }
    return finalMessage;
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

/*
    You can filter out the logs by setting the logFilter to one of the following:
    - NO_FILTER: prints all logs
    - VERBOSE: prints only mass logs for heavy debugging
    - DEBUG: prints only logs for debugging
    - INFO: prints only logs for information
    - WARNING: prints only logs for warnings
    - ERROR: prints only logs for errors
*/
void Log::setLogFilter(LogFilter filter) {
    logFilter = static_cast<char>(filter);
}