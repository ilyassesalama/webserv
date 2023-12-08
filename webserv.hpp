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
#include <stdexcept>


#include "src/headers/RequestParser.hpp"
#include "src/classes/Multiplixer/clientProfile.hpp"
#include "src/classes/Multiplixer/connectionsManager.hpp"
#include "src/classes/Multiplixer/initServer.hpp"
#include "src/classes/Multiplixer/libs.hpp"
#include "src/classes/Multiplixer/tools.hpp"



// -------- start global variables -------
#define LOGGING_ENABLED true
#define FULL_LOGGING_ENABLED true