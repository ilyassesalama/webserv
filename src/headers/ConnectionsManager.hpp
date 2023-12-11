#ifndef CONNECTIONSMANAGER_HPP
#define CONNECTIONSMANAGER_HPP

#include"libs.hpp"
#include"MyServer.hpp"


class ConnectionsManager {
    public:
        ConnectionsManager();
        ~ConnectionsManager();
        
        void socketMonitore();
        void acceptNewIncommingConnections(MyServer *serverId);
        // int recvRequest(int clientFd,MyServer* serverId);
        void addFdToTheSet(int clientFd);
        void changeClinetMonitoringEvent(std::string event, int clientFd);
        void addServerToTheSet(MyServer &serverInstance);
        MyServer* getFdServer(int clientFd);
        void deleteFromFdSet(int clientFd);
    private:
        std::vector<MyServer> serversSet;
        std::vector<struct pollfd> masterFdSet;
        size_t serverCount;
};








#endif