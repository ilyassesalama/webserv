#include "../../../webserv.hpp"

/*
	this function stores the boundary of contentType multipart/form-data in an attribute
*/
void RequestParser::getBoundary(std::string contentType) {

	size_t startPos = contentType.find("boundary=");
	startPos += 9;
	contentType = contentType.substr(startPos);
	size_t EndPos = contentType.find(" ");
	this->boundary = contentType.substr(0, EndPos);
}

/*
	this functions checks whether this request contains boundary end
*/
bool isEndBoundary(std::string &body, std::string boundary) {
	boundary = "--" + boundary + "--";
	size_t boundaryPos = body.rfind(boundary);

	if (boundaryPos == body.size() - boundary.size() - 2)
		return true;

	return false;
}

/*
	this function saves the content of contentType multipart/form-data in a file temporarily
*/
void RequestParser::getBoundaryContent(std::string &body) {

	std::fstream buffer("/tmp/" + this->fileName, std::ios::app);
	if (!buffer.is_open()) {
		this->parsingState.ok = false;
		throw std::runtime_error("Error opening file");
	}

	buffer << body;

	if (isEndBoundary(body, this->boundary))
		this->parsingState.bodyOk = true;

	buffer.close();
}