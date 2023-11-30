#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>


class Network {
    private:
        int port;
    public:
        Network();
        ~Network();

        void startWebServer(void);
};

class RequestParser {
    
};


#endif