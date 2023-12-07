#ifndef CONNECTIONSMANAGER_HPP
#define CONNECTIONSMANAGER_HPP


#include"libs.hpp"
#include"clientProfile.hpp"

class connectionsManager {
    public:
        connectionsManager();
        ~connectionsManager();

        void setListeningSocket(int listenSocket, int port);
        void monitoreSocketsState();
        void acceptNewIncommingConnections(int socketListen);
        void addClientToFdSet(int clientFd);
        void dropClient(int clientIndex, int clientFd);
        int getFileDescriptor(int fdIndex);
        void changeClientMonitoringState(int clientIndex, int stateToMonitore);
    
    private:
       const int backlog;
       nfds_t fds;
       int max_fds;
       std::vector<struct pollfd> pollfds;
       std::vector<struct clientProfile> cProfile;
};




#endif

