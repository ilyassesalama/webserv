#include"connectionsManager.hpp"



connectionsManager::connectionsManager() : backlog(100), max_fds(5), fds(0) {

}

connectionsManager::~connectionsManager() {

}

void connectionsManager::addClientToFdSet(int clientFd) {
    if (clientFd < 0) {
        logs("Invalid Client FD ...");
        return;
    }
    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    (*this).pollfds.push_back(client);
}


void connectionsManager::setListeningSocket(int listenSocket, int port) {
    std::string log_msg = "Listening for incoming connections on port " + std::to_string(port);
    logs(log_msg);
    if(listen(listenSocket,(*this).backlog)) {
        logs("Failed Listening for incoming connections");
        exit(1);
    }
    addClientToFdSet(listenSocket);
}

void connectionsManager::dropClient(int clientIndex, int clientFD) {
    if(clientIndex > 0 && clientIndex < (*this).pollfds.size()) {
        std::vector<struct pollfd>::iterator it = (*this).pollfds.begin() + clientIndex;
        (*this).pollfds.erase(it);
    }
    else if(clientFD > 0) {
        // std::vector<struct pollfd>::iterator it = (*this).pollfds.begin();
        for(std::vector<pollfd>::iterator it = (*this).pollfds.begin(); it != (*this).pollfds.end(); it++) {
            if((*it).fd == clientFD) {
                (*this).pollfds.erase(it);
                break;
            }
        }
    }
    (*this).fds--;
}

void connectionsManager::acceptNewIncommingConnections(int listenSocket) {
    clientProfile client(8080);
    client.address_length = sizeof(client.address);
    client.SocketFD = accept(listenSocket,(struct sockaddr*)&client.address,&client.address_length);
    if(client.SocketFD < 0) {
        logs("accept Failed");
        exit(1);
    }
    addClientToFdSet(client.SocketFD);
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client.address, client.address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    std::string ipAdress(address_buffer);
    client.ipAdress = ipAdress;
    std::string logMsg = "New client connected: " + ipAdress + " on Port 8080 ";
    (*this).cProfile.push_back(client);
    logs(logMsg);
}

int connectionsManager::recvRequest(int clinetFD) {

}

void connectionsManager::monitoreSocketsState() {
    while(true) {
        if(poll((*this).pollfds.data(),(*this).pollfds.size(),-1) < 0) {
            logs("POLL FAILED");
            exit(1);
        }
        std::vector<struct pollfd>::iterator it = pollfds.begin();
        for(int i = 0; i < (*this).pollfds.size(); i++) {
            if((*it).revents & POLLIN) {
                if(it == (*this).pollfds.begin()) {
                    (*this).acceptNewIncommingConnections((*it).fd);
                }
                else {
                    
                }
            }
            it++;
        }
    }
}





int connectionsManager::getFileDescriptor(int fdIndex) {
    return((*this).pollfds[fdIndex].fd);
}

void connectionsManager::changeClientMonitoringState(int clientIndex, int stateToMonitore) {
    (*this).pollfds[clientIndex].events = stateToMonitore;
}