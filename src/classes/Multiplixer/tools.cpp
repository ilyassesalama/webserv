#include"tools.hpp"



void logs(std::string message){
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);

    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

    std::ofstream logFile ("webserverLogs.txt" , std::ios::app);
    if(logFile.is_open()) {
        logFile << "[" << timeString << "] " << message << std::endl;
        std::cout << "[" << timeString << "] " << message << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "Log file not opend" << std::endl;
        std::exit(1);
    }
}