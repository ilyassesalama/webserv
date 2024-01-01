#pragma once

#include "../../webserv.hpp"
class RequestParser;


//prob in the header files UNKOWN TYPE

#include "ClientProfile.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"

struct ClientProfile {
    std::string serverName;
    bool isReceiving;
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionTime;
    int requestCount;
    
    // std::string requestStr;
    std::string responseStr;

    RequestParser request;
    Response response;

	std::string requestBuffered;
	bool isHeaders;
};