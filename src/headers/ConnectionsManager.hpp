#pragma once

#include "../../webserv.hpp"

class ServerInstance;

class ConnectionsManager {
    public:
        ConnectionsManager();
        ~ConnectionsManager();
        
        void socketMonitore();
        void acceptNewIncommingConnections(ServerInstance *serverId);
        // int recvRequest(int clientFd,Server* serverId);
        void addFdToTheSet(int clientFd);
        void changeClientMonitoringEvent(std::string event, int clientFd);
        void addServerToTheSet(ServerInstance &serverInstance);
        ServerInstance* getFdServer(int clientFd);
        void deleteFromFdSet(int clientFd);
        bool isTimeOut(time_t currentTime,  time_t time);
        void checkClientTimeOut();
    private:
        std::vector<ServerInstance> serversSet;
        std::vector<struct pollfd> masterFdSet;
        size_t serverCount;
};