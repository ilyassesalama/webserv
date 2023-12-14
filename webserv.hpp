#pragma once

// keep these here, so we can avoid repeating them in every file
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <netdb.h>
#include <ctime>
#include <vector>
#include <list>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <ctime>

#include "src/headers/ClientProfile.hpp"
#include "src/headers/ConfigurationFile.hpp"
#include "src/headers/ConnectionsManager.hpp"
#include "src/headers/File.hpp"
#include "src/headers/Logger.hpp"
#include "src/headers/RequestParser.hpp"
#include "src/headers/Response.hpp"
#include "src/headers/ServerInstance.hpp"
#include "src/headers/String.hpp"
#include "src/headers/Utils.hpp"


// -------- start global variables -------
#define LOGGING_ENABLED true
#define FULL_LOGGING_ENABLED true

#define FULL_REQUEST_RECEIVED 153
#define INCOMPLETE_REQUEST 154
#define INVALIDE_REQUEST 155
#define DROP_CLIENT 156