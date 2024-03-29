#pragma once

#include "../../webserv.hpp"

class String {
    public:
        static std::string getExtension(const std::string &path);
        static bool endsWith(const std::string &str, const std::string &suffix);
        static int to_int(const std::string &str);
        static size_t to_size_t(const std::string &str);
        static std::string to_string(int num);
        static bool is_a_num(std::string str);
        static bool isIpFormCorrect(std::string ip);
        static std::string to_upper(std::string str);
};