#pragma once

// keep these here, so we can avoid repeating them in every file
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <sys/types.h>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <netdb.h>
#include <ctime>
#include <vector>
#include <fcntl.h>
#include <poll.h>
#include <list>


#include "src/headers/ClientProfile.hpp"
#include "src/headers/ConfigurationFile.hpp"
#include "src/headers/ConnectionsManager.hpp"
#include "src/headers/File.hpp"
#include "src/headers/Logger.hpp"
#include "src/headers/RequestParser.hpp"
#include "src/headers/Response.hpp"
#include "src/headers/ServerInstance.hpp"
#include "src/headers/String.hpp"


// -------- start global variables -------
#define LOGGING_ENABLED true
#define FULL_LOGGING_ENABLED false