#include "../../headers/ConfigurationFile.hpp"

size_t findKeyEnd( std::string file, size_t start,  bool isEnd )
{
	for (size_t i = start; i < file.size(); i++)
		if ( (!isEnd && isspace(file[i])) || (isEnd && (file[i] == ';' || file[i] == '\n')) ) return i;
	return file.size();
}

std::string getDirectiveKey( std::string file, size_t *startIndex ) {

	skipSpaces(file, startIndex);

	size_t endIndex = findKeyEnd(file, *startIndex, false);

	std::string directiveKey = file.substr(*startIndex, endIndex - *startIndex);

	*startIndex = endIndex;

	return directiveKey;
}

std::string getSingleValue( std::string value, size_t *startPos ) {

	if (value.empty()) throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	// size_t i = 0;

	// skipSpaces(value, &i);


	size_t end;

	for (end = 0; end < value.size(); end++)
		if (isspace(value[end]) || value[end] == ';') break ;

	std::string directiveValue = value.substr(0, end);

	skipSpaces(value, &end);

	if (value[end] == ';') end++;
	else throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	if (end + 1 != value.size() && value[end + 1] == ';') throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	*startPos += end;

	return directiveValue;
}

std::string getMultipleValues( std::string value, size_t *startPos ) {

	if (value.empty()) throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	size_t end;

	for (end = 0; end < value.size(); end++)
		if (value[end] == ';') break ;

	std::string directiveValue = value.substr(0, end);

	skipSpaces(value, &end);

	if (value[end] == ';') end++;
	else throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	if (end + 1 != value.size() && value[end + 1] == ';') throw ConfigurationFile::customException((char *)"Error, there is a syntax error");

	*startPos += end;

	return directiveValue;
}

t_listen parseListen( std::string value ) {

	if (valueCounter(value, ':') != 1) throw ConfigurationFile::customException((char *)"Error, listen syntax error");

	size_t pos = value.find(":");


	if (pos == 0 || value.size() == pos || !isDigits(&value[pos + 1])) throw ConfigurationFile::customException((char *)"Error, listen syntax error");
	
	t_listen listen;

	listen.host = value.substr(0, pos);

	std::stringstream buffer(value.substr(pos + 1, value.size() - pos));

	buffer >> listen.port;
	if (buffer.fail()) throw ConfigurationFile::customException((char *)"Error, stringstream");

	return listen;
}

std::string singleValueParser( std::string value ) {

	if (onlySpaces(value) || value.empty()) throw ConfigurationFile::customException((char *)"Error, directive key syntax error");

	std::string directiveValue;

	size_t i = 0;

	skipSpaces(value, &i);

	size_t end;

	for (end = i; end < value.size(); end++)
		if (isspace(value[end])) break ;

	directiveValue = value.substr(i, end - i);

	skipSpaces(value, &end);

	if (end != value.size()) throw ConfigurationFile::customException((char *)"Error, directive key syntax error");

	return directiveValue;
}

std::vector<std::string> multipleValuesParser( std::string value ) {

	std::vector<std::string> values;

	if (onlySpaces(value) || value.empty()) throw ConfigurationFile::customException((char *)"Error, directive key syntax error");

	size_t pos = value.find(" ");

	if (pos == std::string::npos) {
		values.push_back(value);
		return values;
	}

	while (pos != std::string::npos) {
		values.push_back(value.substr(0, pos));

		for (; pos < value.size(); pos++)
			if (value[pos] != ' ') break ;
		value = value.substr(pos);

		if (onlySpaces(value) || value.empty()) break ;

		pos = value.find(" ");
		if (pos == std::string::npos) {
			values.push_back(value);
			break ;
		}
	}

	return values;
}

bool checkUnit(int unit) {
	if (unit != 'K' && unit != 'M' && unit != 'G')
		return false;
	return true;
}

void parseClientBodySize( int *body_size, std::string &body_size_unit, std::string value ) {

	if (onlySpaces(value) || value.empty()) throw ConfigurationFile::customException((char *)"Error, client_body_size syntax error");

	size_t pos = value.find(" ");
	if (pos != std::string::npos && pos == 0)
		skipSpaces(value, &pos);
	else if (pos != std::string::npos && pos != 0) {
		if (!onlySpaces(&value[pos])) throw ConfigurationFile::customException((char *)"Error, client_body_size syntax error");
		else pos = 0;
	}
	else if (pos == std::string::npos)
		pos = 0;

	size_t i;
	for (i = pos; i < value.size(); i++)
		if (!isdigit(value[i])) break ;
	if (i == pos || i == value.size()) throw ConfigurationFile::customException((char *)"Error, client_body_size syntax error");

	std::stringstream buffer(value.substr(pos, i - pos));
	int client_body_size;
	buffer >> client_body_size;
	if (buffer.fail()) throw ConfigurationFile::customException((char *)"Error, stringstream");

	*body_size = client_body_size;
	if (!isspace(value[i]) && checkUnit(value[i]) && onlySpaces(&value[i + 1]))
		body_size_unit = value[i];
	else throw ConfigurationFile::customException((char *)"Error, client_body_size syntax error");
}

std::vector<t_error_page> parseErrorPage( std::string value ) {

	if (onlySpaces(value) || value.empty()) throw ConfigurationFile::customException((char *)"Error, error_page syntax error");
	size_t pos = value.find(" ");

	if (pos == std::string::npos) throw ConfigurationFile::customException((char *)"Error, error_page syntax error");

	std::vector<t_error_page>errors;
	t_error_page err;

	err.error_page = "";

	std::string error_page;

	while (pos != std::string::npos) {
		if (!isDigits(value.substr(0, pos))) throw ConfigurationFile::customException((char *)"Error, error_page syntax error");
		std::stringstream buffer(value.substr(0, pos));
		int errorCode;
		buffer >> errorCode;
		if (buffer.fail()) throw ConfigurationFile::customException((char *)"Error, stringstream");
		err.error_code = errorCode;

		for (; pos < value.size(); pos++)
			if (value[pos] != ' ') break ;
		value = value.substr(pos);

		if ((onlySpaces(value) || value.empty()) && err.error_page.empty()) throw ConfigurationFile::customException((char *)"Error, error_page syntax error");
		else if ((onlySpaces(value) || value.empty()) && !err.error_page.empty()) break ;

		pos = value.find(" ");

		errors.push_back(err);

		if (pos == std::string::npos) {
			error_page = value;
			break ;
		}
	}

	std::vector<t_error_page>::iterator it;

	for (it = errors.begin(); it != errors.end(); it++)
		it->error_page = error_page;

	return errors;
}

std::string getDirectiveValue(std::string key, std::string file, size_t *startIndex) {
	return key != "error_page" && key != "server_name" ? getSingleValue(file, startIndex) : getMultipleValues(file, startIndex);
}