#include "../../../webserv.hpp"

bool findVal( std::vector<t_error_page>error_pages, int val ) {
	
	std::vector<t_error_page>::iterator it;

	for (it = error_pages.begin(); it != error_pages.end(); it++)
		if (it->error_code == val) return true;
	return false;
}

void ConfigurationFile::parseValue( std::string key, std::string value, t_server *server ) {
	
	if (key == "listen") {
		if (!server->is_listen) {
			t_listen listen = parseListen( value );
			server->listen = listen;
			server->is_listen = true;
		} else {
			Log::w("Warning, there is more than one listen directive");
		}
	}
	else if (key == "server_name") {
		if (!server->is_server_name) {
			server->server_name = singleValueParser( value );
			server->is_server_name = true;
		} else {
			Log::w("Warning, there is more than one server_name directive");
		}
	} else if (key == "client_body_size") {
		if (!server->is_client_body_size) {
			parseClientBodySize(&server->client_body_size, server->body_size_unit, value);
			server->is_client_body_size = true;
		} else {
			Log::w("Warning, there is more than one client_body_size directive");
		}
	} else if (key == "error_page") {

		std::vector<t_error_page>errors = parseErrorPage( value );

		std::vector<t_error_page>::iterator it;
		for (it = errors.begin(); it != errors.end(); it++) {
			if (!findVal(server->error_pages, it->error_code)) {
				if(access(it->error_page.c_str(), R_OK) == -1) {
					Log::w("The custom error page " + String::to_string(it->error_code) + " you specified in the config file isn't accessible. We'll use our own error page.");
				} else {
					server->error_pages.push_back(*it);
				}
			}
		}
	}
	else if (key != "listen" && key != "server_name" && key != "client_body_size" && key != "error_page") {
		throw (Utils::WebservException("Error, directive not allowed"));
	}
}

void ConfigurationFile::parseRouteValue( std::string key, std::string value, t_route *route) {

	if (key == "root") {
		if (!route->is_root) {
			route->root = singleValueParser(value);
			if (route->root[0] != '/')
				throw (Utils::WebservException("Error, root directive must start with a slash"));
			if (!String::endsWith(route->root, "/"))
				route->root.append("/");
			route->is_root = true;
		} else {
			Log::w("Warning, there is more than one root directive");
		}
	} else if (key == "directory_listing") {
		if (!route->is_directory_listing) {
			route->directory_listing = boolParser(value);
			route->is_directory_listing = true;
		} else {
			Log::w("Warning, there is more than one directory_listing directive");
		}
	} else if (key == "index") {
		if (!route->is_index) {
			route->index = singleValueParser(value);
			route->is_index = true;
		} else {
			Log::w("Warning, there is more than one index directive");
		}
	} else if (key == "cgi_extension") {
		if (!route->is_cgi_extension) {
			route->cgi_extension = multipleValuesParser(value);
			route->is_cgi_extension = true;
		} else {
			Log::w("Warning, there is more than one cgi_extension directive");
		}
	} else if (key == "allowed_methods") {
		if (!route->is_allowed_methods) {
			route->allowed_methods = multipleValuesParser(value);
			checkAllowedMethods(route->allowed_methods);
			route->is_allowed_methods = true;
		} else {
			Log::w("Warning, there is more than one allowed_methods directive");
		}
	} else if (key == "redirection") {
		if (!route->is_redirection) {
			route->redirection = parseRedirect(value);
			route->is_redirection = true;
		} else {
			Log::w("Warning, there is more than one redirection directive");
		}
	} else if (key == "upload") {
		if (!route->is_upload) {
			route->upload = boolParser(value);
			route->is_upload = true;
		} else {
			Log::w("Warning, there is more than one upload directive");
		}
	} else if (key == "upload_path") {
		if (!route->is_upload_path) {
			route->upload_path = singleValueParser(value);
			if (route->upload_path[0] != '/')
				throw (Utils::WebservException("Error, upload path directive must start with a slash"));
			if (!String::endsWith(route->upload_path, "/"))
				route->upload_path.append("/");
			route->is_upload_path = true;
		} else {
			Log::w("Warning, there is more than one upload_path directive");
		}
	} else if (key == "cgi") {
		if (!route->is_isCGI) {
			route->isCGI = boolParser(value);
			route->is_isCGI = true;
		} else {
			Log::w("Warning, there is more than one cgi directive");
		}
	} else {
		throw (Utils::WebservException ("Error, directive not allowed"));
	}
}

void ConfigurationFile::handleLocation( std::string file, size_t *startIndex, std::string path, t_server *server ) {

	std::string directiveKey;
	std::string directiveValue;

	size_t endIndex = getLocationEnd(file);

	size_t i;

	t_route route;

	route.path = path;
	route.directory_listing = false;
	route.isCGI = false;
	route.upload = false;

	initRouteBooleans(&route);
	for (i = 0; i < endIndex && file[i] != '}'; i++) {
		skipSpaces(file, &i);

		if (file[i] == '}') break ;

		directiveKey = getDirectiveKey( file, &i );

		skipSpaces(file, &i);

		directiveValue = getRouteDirectiveValue(directiveKey, &file[i], &i);

		parseRouteValue(directiveKey, directiveValue, &route);
	}

	if (!route.is_root)
		throw (Utils::WebservException("Error, there is no root directive in one of the location blocks"));

	if (route.is_upload_path && !File::isDirectory(File::getWorkingDir() + route.root + route.upload_path))
		throw (Utils::WebservException("Error, upload_path must be an existing directory in client-side" + route.root + " folder and should start with a slash"));

	server->routes.push_back(route);

	if (file[i] && file[i] == '}') i++;

	*startIndex += i;
}

void ConfigurationFile::handleDirectives( std::string file, t_server server ) {

	size_t endIndex = serverStartPosition( file );
	if (endIndex == std::string::npos) endIndex = file.size();

	size_t startIndex = findOpeningBrace( file, endIndex );

	std::string directiveKey;
	std::string directiveValue;

	server.is_client_body_size = false;
	server.is_listen = false;
	server.is_server_name = false;

	for (size_t i = ++startIndex; i < endIndex; i++) {

		skipSpaces(file, &i);

		if (file[i] == '}') break ;

		directiveKey = getDirectiveKey( file, &i );

		skipSpaces(file, &i);

		if (directiveKey != "location") {
			directiveValue = getDirectiveValue(directiveKey, &file[i], &i);

			parseValue(directiveKey, directiveValue, &server);
		}
		else {

			directiveValue = getLocationPath(&file[i], &i);

			if (isRouteAlreadyExist(server, directiveValue)) {
				skipRoute(&file[i], &i);
				Log::w("Warning, there is already a route with the same location path this one is going to be ignored");
			} else {
				skipSpaces(file, &i);

				if (file[i] != '{') throw (Utils::WebservException ("Error, there is no opening brace after location directive"));

				i++;

				handleLocation( &file[i], &i, directiveValue, &server );
	
				if (file[i] == '{') throw (Utils::WebservException ("Error, opening brace at the end of location directive"));
			}
		}

	}

	if (!server.is_listen) {
		throw (Utils::WebservException ("Error, there is no listen directive"));
	}

	if (!server.is_server_name) {
		throw (Utils::WebservException("Error, server_name directive must have at least one value"));
	}

	if (server.routes.empty()) {
		throw (Utils::WebservException("Error, each server must have at least one location block"));
	}
	
	if (checkDuplicateServers(this->ConfigFileServers, server))
		this->ConfigFileServers.push_back(server);
	else
		Log::w("Warning, there is already a server with the same listen & server_name values");
}

void ConfigurationFile::configFileParsing( void ) {

	std::string tmpFile = this->file;

	size_t serverPos = serverStartPosition( tmpFile );
	if (serverPos == std::string::npos) throw(Utils::WebservException("Error, there is no server block"));

	for (size_t i = serverPos == 0 ? serverPos : serverPos - 1; i > 0; i--)
		if (!isspace(tmpFile[i]))
			throw(Utils::WebservException("Error, there is a syntax error"));
	
	size_t startIndex;
	size_t endIndex;

	t_server server;

	while (serverPos != std::string::npos) {

		getServerCoords(&tmpFile[serverPos + strlen("server")], &startIndex, &endIndex);

		curlyBracesChecker(&tmpFile[serverPos + strlen("server")], startIndex, endIndex);

		handleDirectives(&tmpFile[serverPos + strlen("server")], server);

		checkBetweenServers(&tmpFile[serverPos + strlen("server")], endIndex);

		tmpFile = tmpFile.substr(serverPos + strlen("server"));
		serverPos = serverStartPosition(tmpFile);
	}

	if (this->ConfigFileServers.empty()) {
		throw(Utils::WebservException("Error, there is no server block"));
	}

}

ConfigurationFile::ConfigurationFile( std::string file ) {
	try {
		this->file = File::getFileContent(file);

		if (this->file.empty() || onlySpaces(this->file)) throw(Utils::WebservException("Error, config file is empty"));

		this->configFileParsing();

		setDefaultErrors(this->getConfigFileServers());
	} catch (std::exception &e) {
		Log::e("Caught exception due to: " + std::string(e.what()));
		exit(1);
	}
}

std::list<t_server>& ConfigurationFile::getConfigFileServers() {
	return(this->ConfigFileServers);
}