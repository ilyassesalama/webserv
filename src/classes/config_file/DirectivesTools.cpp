#include "../../../webserv.hpp"

size_t findKeyEnd(std::string file, size_t start, bool isEnd)
{
	for (size_t i = start; i < file.size(); i++)
		if ((!isEnd && isspace(file[i])) || (isEnd && (file[i] == ';' || file[i] == '\n')))
			return i;
	return file.size();
}

std::string getDirectiveKey(std::string file, size_t *startIndex)
{

	skipSpaces(file, startIndex);

	size_t endIndex = findKeyEnd(file, *startIndex, false);

	std::string directiveKey = file.substr(*startIndex, endIndex - *startIndex);

	*startIndex = endIndex;

	return directiveKey;
}

std::string getSingleValue(std::string value, size_t *startPos)
{

	if (value.empty()) throw (Utils::WebservException(Utils::WebservException("Error, there is a syntax error")));

	size_t end;

	for (end = 0; end < value.size(); end++)
		if (isspace(value[end]) || value[end] == ';') break ;

	std::string directiveValue = value.substr(0, end);

	skipSpaces(value, &end);

	if (value[end] == ';') end++;
	else throw(Utils::WebservException("Error, there is more than one value for directive or it does not end with a semi-colon"));

	if (end + 1 != value.size() && value[end + 1] == ';') throw(Utils::WebservException("Error, there is more than one semi-colon at directive end"));

	*startPos += end;

	return directiveValue;
}

std::string getMultipleValues(std::string value, size_t *startPos)
{

	if (value.empty()) throw(Utils::WebservException("Error, there is a syntax error"));

	size_t end;

	for (end = 0; end < value.size(); end++)
		if (value[end] == ';') break ;

	std::string directiveValue = value.substr(0, end);

	skipSpaces(value, &end);

	if (value[end] == ';') end++;
	else throw(Utils::WebservException("Error, there is a syntax error"));

	if (end + 1 != value.size() && value[end + 1] == ';') throw(Utils::WebservException("Error, there is a syntax error"));

	*startPos += end;

	return directiveValue;
}

t_listen parseListen(std::string value)
{

	if (valueCounter(value, ':') != 1) throw(Utils::WebservException("Error, listen syntax error"));

	size_t pos = value.find(":");


	if (pos == 0 || value.size() == pos || !isDigits(&value[pos + 1])) throw(Utils::WebservException("Error, listen syntax error"));
	
	t_listen listen;

	listen.host = value.substr(0, pos);

	std::stringstream buffer(value.substr(pos + 1, value.size() - pos));

	buffer >> listen.port;
	if (buffer.fail()) throw(Utils::WebservException("Error, stringstream"));

	return listen;
}

std::string singleValueParser(std::string value)
{

	if (onlySpaces(value) || value.empty()) throw(Utils::WebservException("Error, directive key syntax error"));

	std::string directiveValue;

	size_t i = 0;

	skipSpaces(value, &i);

	size_t end;

	for (end = i; end < value.size(); end++)
		if (isspace(value[end])) break ;

	directiveValue = value.substr(i, end - i);

	skipSpaces(value, &end);

	if (end != value.size()) throw(Utils::WebservException("Error, directive key syntax error"));

	return directiveValue;
}

std::vector<std::string> multipleValuesParser(std::string value)
{

	std::vector<std::string> values;

	if (onlySpaces(value) || value.empty()) throw(Utils::WebservException("Error, directive key syntax error"));

	size_t pos = value.find(" ");

	if (pos == std::string::npos) {
			values.push_back(value);
			return values;
	}

	while (pos != std::string::npos) {
			values.push_back(value.substr(0, pos));

			for (; pos < value.size(); pos++)
				if (value[pos] != ' ')
					break;
			value = value.substr(pos);

			if (onlySpaces(value) || value.empty())
				break;

			pos = value.find(" ");
			if (pos == std::string::npos)
			{
				values.push_back(value);
				break;
			}
	}

	return values;
}

bool checkUnit(int unit)
{
	if (unit != 'K' && unit != 'M' && unit != 'G')
		return false;
	return true;
}

size_t convertFromUnitToBytes(size_t body_size, std::string body_size_unit){
	if (body_size_unit == "K")
		return body_size * 1024;
	else if (body_size_unit == "M")
		return body_size * 1024 * 1024;
	else if (body_size_unit == "G")
		return body_size * 1024 * 1024 * 1024;
	return body_size;
}

void parseClientBodySize(size_t *body_size, std::string &body_size_unit, std::string value)
{

	if (onlySpaces(value) || value.empty()) throw(Utils::WebservException("Error, client_body_size syntax error"));

	size_t pos = value.find(" ");
	if (pos != std::string::npos && pos == 0)
		skipSpaces(value, &pos);
	else if (pos != std::string::npos && pos != 0) {
			if (!onlySpaces(&value[pos])) throw(Utils::WebservException("Error, client_body_size syntax error"));
		else pos = 0;
	}
	else if (pos == std::string::npos)
		pos = 0;

	size_t i;
	for (i = pos; i < value.size(); i++)
		if (!isdigit(value[i])) break ;
	if (i == pos || i == value.size()) throw(Utils::WebservException("Error, client_body_size syntax error"));

	std::stringstream buffer(value.substr(pos, i - pos));
	size_t client_body_size;
	buffer >> client_body_size;
	if (buffer.fail()) throw(Utils::WebservException("Error, stringstream"));

	if (!isspace(value[i]) && checkUnit(value[i]) && onlySpaces(&value[i + 1])){
		body_size_unit = value[i];
		*body_size = convertFromUnitToBytes(client_body_size, body_size_unit);
	}
	else throw(Utils::WebservException("Error, client_body_size syntax error"));
}

std::vector<t_error_page> parseErrorPage(std::string value)
{

	if (onlySpaces(value) || value.empty()) throw(Utils::WebservException("Error, error_page syntax error"));

	std::vector<t_error_page>errors;
	t_error_page err;

	err.error_page = "";

	std::string error_page;

	size_t pos = value.find(" ");
	if (pos == std::string::npos || onlySpaces(value.substr(pos))) throw(Utils::WebservException("Error, error_page syntax error"));

	while (pos != std::string::npos) {
		if (!isDigits(value.substr(0, pos))) throw(Utils::WebservException("Error, error_page syntax error"));
		
		int errorCode = String::to_int(value.substr(0, pos));

		if (!checkErrorCode(errorCode)) throw(Utils::WebservException("Error, error_code not allowed"));
		err.error_code = errorCode;

		value = value.substr(pos);
		pos = 0;
		skipSpaces(value, &pos);
		value = value.substr(pos);

		errors.push_back(err);

		if (value.empty()) throw(Utils::WebservException("Error, error_page syntax error"));

		pos = value.find(" ");

		if (pos == std::string::npos || onlySpaces(value.substr(pos))) {
			error_page = value.substr(0, pos);
			break ;
		}
	}

	std::vector<t_error_page>::iterator it;

	for (it = errors.begin(); it != errors.end(); it++) {
		it->error_page = error_page;
	}

	return errors;
}

std::string getDirectiveValue(std::string key, std::string file, size_t *startIndex)
{
	return key != "error_page" ? getSingleValue(file, startIndex) : getMultipleValues(file, startIndex);
}
