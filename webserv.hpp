#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

class Network {
    public:
        Network();
        ~Network();
};


// -------- start request-related code -------
class RequestParser {
    public:
        void initRequestParser(std::string requestData);
};
// -------- end request-related code -------



#endif