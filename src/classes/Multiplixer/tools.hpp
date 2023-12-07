#ifndef TOOLS_HPP
#define TOOLS_HPP

#include"libs.hpp"

class Log {
    public:
        static std::string formatLoggingMessage(std::string message);
        static void d(std::string message); // debug
        static void i(std::string message); // info
        static void w(std::string message); // warning
        static void e(std::string message); // error
};


#endif