#pragma once

#include "../../webserv.hpp"

enum LogFilter {
    VERBOSE = 'v',
    DEBUG = 'd',
    INFO = 'i',
    WARNING = 'w',
    ERROR = 'e',
    NO_FILTER = '0'
};

class Log {
    public:
        static std::string formatLoggingMessage(std::string message);
        static void setLogFilter(LogFilter filter);
        static void v(std::string message); // verbose
        static void d(std::string message); // debug
        static void i(std::string message); // info
        static void w(std::string message); // warning
        static void e(std::string message); // error
    private:
        static bool isInitialized;
        static char logFilter;
};

void setSocketNonBlocking(int socketFd);