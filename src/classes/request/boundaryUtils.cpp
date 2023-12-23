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
	size_t endPos = body.rfind(boundary);
	size_t CRLF = body.rfind("\r\n");
	if (CRLF == std::string::npos) {
		CRLF = body.rfind("\n");
		if (CRLF == std::string::npos)
			CRLF = 0;
		else if (CRLF == body.size() - 1)
			CRLF = 1;
	} else {
		if (CRLF == body.size() - 2)	
			CRLF = 2;
		else
			CRLF = 0;
	}
	if (endPos == std::string::npos)
		return false;
	else if (endPos == body.size() - boundary.size() - CRLF)
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
		return ;
	}

	buffer << body;

	if (isEndBoundary(body, this->boundary)) {
		this->parsingState.bodyOk = true;
	}

	buffer.close();
}