#include "../../../webserv.hpp"

ConnectionsManager::ConnectionsManager() {
    this->serverCount = 0;
}

ConnectionsManager::~ConnectionsManager() {

}

void ConnectionsManager::addServerToTheSet(ServerInstance &serverInstance) {
    this->serversSet.push_back(serverInstance);
    addFdToTheSet(serverInstance.getListenSocketFd());
    this->serverCount++;

    // std::vector<struct pollfd> clientFdSet = serverInstance.getClientFdSet();
    // std::vector<struct pollfd>::iterator it = clientFdSet.begin();

    // this->masterFdSet.push_back((*it));

}
void ConnectionsManager::deleteFromFdSet(int clientFd) {
    for(std::vector<struct pollfd>::iterator it = this->masterFdSet.begin(); it != this->masterFdSet.end(); it++) {
        if(it->fd == clientFd) {
            this->masterFdSet.erase(it);
            // close(clientFd);
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
    this->masterFdSet.push_back(client);
}


bool ConnectionsManager::isTimeOut(time_t currentTime,  time_t time) {
    if(currentTime - time < 30) {
        return(false);
    }
    return(true);

}

void ConnectionsManager::checkClientTimeOut() {
    ClientProfile *client;
    ServerInstance *serverId;
    time_t currentTime = std::time(0);
    std::vector<struct pollfd> tmp (this->masterFdSet.begin() + this->serverCount , this->masterFdSet.end());
    for(std::vector<struct pollfd>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        serverId = getFdServer(it->fd);
        client = serverId->getClientProfile(it->fd);
        if(isTimeOut(currentTime, client->connectionTime)) {
            serverId->dropClient(it->fd);
            this->deleteFromFdSet(it->fd);
        }
    }
}

void ConnectionsManager::acceptNewIncommingConnections(ServerInstance *serverId) {
    ClientProfile client;

	client.requestBuffered = "";
	client.isHeaders = true;

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
    client.connectionTime = std::time(0);
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

void ConnectionsManager::changeClientMonitoringEvent(std::string event, int clientFd) {
    for(std::vector<struct pollfd>::iterator it = this->masterFdSet.begin(); it != this->masterFdSet.end(); it++) {
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

bool ConnectionsManager::isaListeningSocket(int fd) {
    for(std::vector<ServerInstance>::iterator it = this->serversSet.begin(); it != this->serversSet.end(); it++) {
        if(fd == it->getListenSocketFd())
            return(true);
    }
    return(false);
}

void ConnectionsManager::socketMonitore() {
    int pollResult;
    while (true) {
        checkClientTimeOut();
        pollResult = poll(&masterFdSet[0], masterFdSet.size(), -1);
        if (pollResult < 0) {
            Log::e("Poll Failed ...");
            exit(1);
        }
        for (std::vector<struct pollfd>::iterator it = masterFdSet.begin(); it != masterFdSet.end(); ++it) {
            if (it->revents & POLLIN) {
                if (isaListeningSocket(it->fd) == true) {
                    acceptNewIncommingConnections(getFdServer(it->fd));
                    this->masterFdSet[0].events = POLLIN;
                    this->masterFdSet[0].revents = 0;
                    break;
                } else {
                    int requestState = getFdServer(it->fd)->receiveRequest(it->fd);
                    if (requestState == FULL_REQUEST_RECEIVED) {
                        changeClientMonitoringEvent("write", it->fd);
                    } else if (requestState == DROP_CLIENT) {
                        this->deleteFromFdSet(it->fd);
                        break; 
                    }
                    it->revents = 0;
                }
            }
            if (it->revents & POLLOUT) {
                if (getFdServer(it->fd)->sendResponse(it->fd) == 1)
                    changeClientMonitoringEvent("read", it->fd);
                it->revents = 0;
            }
        }
    }
}

size_t ConnectionsManager::getServerCount() {
    return((*this).serverCount);
}

ServerInstance* ConnectionsManager::isServerExist(ServerInstance &server) {
    for (std::vector<ServerInstance>::iterator it = this->serversSet.begin(); it != serversSet.end(); it++) {
        if(it->getListenAdress() == server.getListenAdress() && it->getServerPort() == server.getServerPort())
            return(&(*it));
    }
    return(NULL);
}


void ConnectionsManager::printMasterFdSet() {
    for(std::vector<struct pollfd>::iterator it = this->masterFdSet.begin(); it != this->masterFdSet.end(); it++) {
        std::cout << "client Fd : " << it->fd << " client event : " << it->events << " client revent : " << it->revents << std::endl;
    }
}

void ConnectionsManager::printServers() {
    for(std::vector<ServerInstance>::iterator it = this->serversSet.begin(); it != this->serversSet.end(); it++) {
        std::cout <<"Server name : "<< it->getServerName() <<  " listen Socket Fd : " << it->getListenSocketFd() << std::endl;
    }
}






