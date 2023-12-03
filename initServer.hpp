#ifndef INITSERVER_HPP
#define INITSERVER_HPP

#include"libs.hpp"


class initServer {
    public:
        initServer(int port);
        int create_listening_socket();
        int bind_listenning_socket();
        int start_listenning();
        int getSocket();
        initServer();
        ~initServer();
    private:
        struct addrinfo hint;
        struct addrinfo *bind_address;
        int socket_listen;
};



#endif