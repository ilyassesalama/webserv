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

#include "src/classes/request/RequestParser.hpp"
#include "src/classes/response/Response.hpp"
#include "src/classes/utils/File.hpp"
#include "src/classes/utils/String.hpp"
#include "src/classes/multiplixer/clientProfile.hpp"
#include "src/classes/multiplixer/connectionsManager.hpp"
#include "src/classes/multiplixer/initServer.hpp"
#include "src/classes/multiplixer/libs.hpp"
#include "src/classes/multiplixer/tools.hpp"

// -------- start global variables -------
#define LOGGING_ENABLED true
#define FULL_LOGGING_ENABLED false