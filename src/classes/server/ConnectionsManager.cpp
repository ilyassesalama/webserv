#include "../../../webserv.hpp"

ConnectionsManager::ConnectionsManager() {
    (*this).serverCount = 0;
}

ConnectionsManager::~ConnectionsManager() {

}

void ConnectionsManager::addServerToTheSet(ServerInstance &serverInstance) {
    (*this).serversSet.push_back(serverInstance);
    (*this).serverCount++;

    std::vector<struct pollfd> clientFdSet = serverInstance.getClientFdSet();
    std::vector<struct pollfd>::iterator it = clientFdSet.begin();

    (*this).masterFdSet.push_back((*it));

}

void ConnectionsManager::deleteFromFdSet(int clientFd) {
    for(std::vector<struct pollfd>::iterator it = (*this).masterFdSet.begin(); it != (*this).masterFdSet.end(); it++) {
        if(it->fd == clientFd) {
            (*this).masterFdSet.erase(it);
            close(clientFd);
            break;
        }
    }
}

void ConnectionsManager::addFdToTheSet(int clientFd) {
    if (clientFd < 0) {
        Log::e("Invalid Client FD ...");
        return;
    }
    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    (*this).masterFdSet.push_back(client);
}

void ConnectionsManager::acceptNewIncommingConnections(ServerInstance *serverId) {
    ClientProfile client;

    client.address_length = sizeof(client.address);
    client.SocketFD = accept(serverId->getListenSocketFd(),(struct sockaddr*)&client.address,&client.address_length);
    if(client.SocketFD < 0) {
        Log::e("accept Failed ...");
        exit(1);
    }
    setSocketNonBlocking(client.SocketFD);
    addFdToTheSet(client.SocketFD);
    serverId->AddFdToPollFds(client.SocketFD);
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client.address, client.address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    std::string ipAdress(address_buffer);
    client.ipAdress = ipAdress;
    serverId->AddToClientProfiles(client);
    Log::d("New client connected: " + ipAdress + " on Port " + serverId->getServerPort());
}


ServerInstance* ConnectionsManager::getFdServer(int clientFd) {
    for (std::vector<ServerInstance>::iterator it = serversSet.begin(); it != serversSet.end(); it++) {
        if (it->isClientFdInPollFd(clientFd)) {
            return &(*it);
        }
    }
    return NULL; 
}

void ConnectionsManager::changeClinetMonitoringEvent(std::string event, int clientFd) {
    for(std::vector<struct pollfd>::iterator it = (*this).masterFdSet.begin(); it != (*this).masterFdSet.end(); it++) {
        if(it->fd == clientFd) {
            if(event == "write") {
                it->events = POLLOUT;
                it->revents = 0;
            } 
            else if(event == "read") {
                it->events = POLLIN;
                it->revents = 0;
            }
            break;
        }
    }
}

void ConnectionsManager::socketMonitore() {
    while (true) {
        if (poll(&masterFdSet[0], masterFdSet.size(), 0) < 0) {
            Log::e("POLL FAILED ...");
            exit(1);
        }
        std::vector<struct pollfd>::iterator it = masterFdSet.begin();
        for (size_t i = 0; i < (*this).masterFdSet.size(); i++) {
            if (it->revents & POLLIN) {
                if (i < (*this).serverCount) {
                    acceptNewIncommingConnections(getFdServer(it->fd));
                } else {
                    int requestState = getFdServer(it->fd)->recvRequest(it->fd);
                    if(requestState == FULL_REQUEST_RECEIVED) {
                        changeClinetMonitoringEvent("write", it->fd);
                    }
                    else if(requestState == DROP_CLIENT) {
                        (*this).deleteFromFdSet(it->fd);
                    }
                }
            }
            if(it->revents & POLLOUT) {
                // getFdServer(it->fd)->sendResponse(it->fd);
                changeClinetMonitoringEvent("read", it->fd);
            }
            it++;
        }
    }
}





