#include "../../../webserv.hpp"

size_t serverStartPosition( std::string file ) {
	size_t pos = file.find("server ");
	return (pos == std::string::npos ? file.find("server{") : pos);
}

void getServerCoords(std::string file, size_t *startIndex, size_t *endIndex) {

	// Here I am checking the next server block position if there is only one I set it to file size
	*endIndex = serverStartPosition(file);
	if (*endIndex == std::string::npos) *endIndex = file.size();

	for (*startIndex = 0; *startIndex < *endIndex; (*startIndex)++) {
		if (file[*startIndex] != '{' && !isspace(file[*startIndex])) throw(Utils::WebservException("Error, there is a syntax error"));
		else if (file[*startIndex] == '{') break ;
	}

	if (*startIndex == *endIndex || file[*startIndex] != '{') throw(Utils::WebservException("Error, there is a syntax error"));
}

size_t findOpeningBrace( std::string file, size_t endIndex ) {

	size_t startIndex;
	
	for (startIndex = 0; startIndex < endIndex; startIndex++) {
		if (file[startIndex] != '{' && !isspace(file[startIndex])) throw(Utils::WebservException("Error, there is a syntax error"));
		else if (file[startIndex] == '{') break ;
	}

	if (startIndex == endIndex || file[startIndex] != '{') throw(Utils::WebservException("Error, there is a syntax error"));

	return startIndex;
}

void skipSpaces( std::string str, size_t *idx )
{
	while (str[*idx] && isspace(str[*idx]))
		(*idx)++;
}

bool onlySpaces( std::string str ) {
	for (size_t i = 0; i < str.size(); i++)
		if (!isspace(str[i])) return false;
	return true;
}

int valueCounter( std::string str, int value ) {
	int counter = 0;

	for (size_t i = 0; i < str.size(); i++)
		if (str[i] == value) counter++;
	return counter;
}

bool isDigits( std::string value ) {
	for (size_t i = 0; i < value.size(); i++)
		if (!isdigit(value[i])) return false;
	return true;
}

bool checkErrorCode(int errorCode) {
	int errorCodes[10] = {301, 400, 403, 404, 405, 409, 413, 414, 500, 501};

	for (size_t i = 0; i < 10; i++)
		if (errorCode == errorCodes[i]) return 1;
	return 0;
}

void setDefaultErrors(std::list<t_server> &servers) {
	(void) servers;
	// std::list<t_server>::iterator it;
	// std::vector<t_error_page>::iterator errorsIt;
	// int errorCodes[10] = {301, 400, 403, 404, 405, 409, 413, 414, 500, 501};

	// for (it = servers.begin(); it != servers.end(); it++) {

	// 	for (errorsIt = it->error_pages.begin(); errorsIt != it->error_pages.end(); errorsIt++) {
	// 		if (checkErrorCode(errorsIt->error_code) )
	// 	}

	// }

}