#include"clientProfile.hpp"





clientProfile::clientProfile(int portNumber) {
    (*this).port = portNumber;
    (*this).isConnected = true;
    time(&(*this).connectionsTime);
    (*this).requestCount = 0;
}

void clientProfile::incRequestCount() {
    (*this).requestCount++;
}