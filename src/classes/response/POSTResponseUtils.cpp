#include "../../../webserv.hpp"

std::string getType(std::map<std::string, std::string>&headers) {
	size_t endPos = headers["Content-Type"].find(";");
	return headers["Content-Type"].substr(0, endPos);
}

std::string getContentType(std::map<std::string, std::string>&headers, std::string path) {
	if (headers.find("Content-Type") == headers.end()) {
		return File::getContentType(path);
	}
	std::string type = getType(headers);
	return type;
}