#ifndef INITSERVER_HPP
#define INITSERVER_HPP

#include"libs.hpp"



class initServer {
    public:
        initServer();
        ~initServer();

        void startServer(int port);
        void printBanner();
        void setPort(int port);
        int getPort();
        void setListenSocket();
        int getListenSocket();
    private:
        int port;
        int listenSocket;
        struct addrinfo hint; //this is where the needed info will be stored
        struct addrinfo *bind_address;
};




#endif