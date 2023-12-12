#pragma once

// #include "../../webserv.hpp"
// class RequestParser;


//prob in the header files UNKOWN TYPE

#include"ClientProfile.hpp"
#include"RequestParser.hpp"


struct ClientProfile {
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionsTime;
    int requestCount;
    
    std::string request;
    std::string response;

    RequestParser parser;
};