#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

// Side note: this fixes `exit()` failing to be resolved!
#include <cstdlib>
// Since the subject implies that we should rely on using C++ish stuff. I added `cstdlib` header.

// According to the subject:
// Try to always develop using the most C++ features you can (for example, choose
// <cstring> over <string.h>). You are allowed to use C functions, but always prefer
// their C++ versions if possible.

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