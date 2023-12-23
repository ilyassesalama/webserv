#include "../../../webserv.hpp"

/*
	returns remaining chunk size of previous request
	if chunk size is not less than request size it returns request size
*/
size_t getChunkEnd(std::string body, size_t chunkRemainder) {

	size_t i;
	for (i = 0; i < body.size(); i++) {
		if (chunkRemainder == 0)
			return i;
		chunkRemainder--;
	}
	return i;
}

/*
	returns chunk end because there may be two chunks in one request or this request ends with 0
*/
size_t getChunkedRequestBodyEnd(std::string &body, size_t chunkRemainder) {

	size_t endPos = body.rfind("\r\n0\r\n\r\n");
	if (endPos == std::string::npos) {
		endPos = body.rfind("\n0\n\n");
		if (endPos != std::string::npos && endPos == body.size() - 4)
			return endPos;
		else
			return getChunkEnd(body, chunkRemainder);
	} else {
		if (endPos != std::string::npos && endPos == body.size() - 7)
			return endPos;
	}
	return getChunkEnd(body, chunkRemainder);
}

/*
	this function subtracts the size of the next chunk from the request body
	the main function only calls this when chunk remainder is zero
*/
size_t getChunkSize(std::string body) {

	size_t pos = body.find("\r\n");
	if (pos == std::string::npos)
		pos = body.find("\n");

	std::string firstLine = body.substr(0, pos);


	size_t hexSize;
	std::stringstream buffer(firstLine);
	buffer >> std::hex >> hexSize;

	return hexSize;
}

/*
	this function checks whether the request contains 0 which means end of request
*/
size_t getZero(std::string &body) {
	size_t zeroPos = body.rfind("\r\n0\r\n\r\n");
	if (zeroPos == std::string::npos) {
		zeroPos = body.rfind("\n0\n\n");
		if (zeroPos != std::string::npos && zeroPos == body.size() - 4)
			return zeroPos;
		else
			return std::string::npos;
	} else {
		if (zeroPos != std::string::npos && zeroPos == body.size() - 7)
			return zeroPos;
		else
			return std::string::npos;
	}
	return zeroPos;
}

/*
	this is the main function to get chunks and it works using recursion
*/
void RequestParser::getChunkedData(std::string &body) {

	// try {
		if (this->chunkRemainder == 0) {
		this->chunkRemainder = getChunkSize(body);
		size_t pos = body.find("\r\n"); // removing chunk size from body
		if (pos == std::string::npos) {
			pos = body.find("\n");
			pos += 1;
		} else {
			pos += 2;
		}
		body = body.substr(pos);
	}

	std::fstream myFile(File::getWorkingDir() + "/tmp1.mp4", std::ios::binary | std::ios::app);
	if (!myFile.is_open()) {
		this->parsingState.ok = false;
		return ;
	}

	bool isZero = getZero(body) == std::string::npos ? false : true;

	size_t bodyEnd = getChunkedRequestBodyEnd(body, this->chunkRemainder);

	std::string chunkStr = body.substr(0, bodyEnd);

	myFile << chunkStr;

	this->chunkRemainder -= chunkStr.size();

	body = body.substr(bodyEnd);

	myFile.close();

	if (isZero == true && this->chunkRemainder == 0) {
		this->parsingState.bodyOk = true;
		this->parsingState.failCode = 201;
		return ;
	}

	if (body.size() != 0) {

		size_t pos = body.find("\r\n"); // removing chunk size from body
		if (pos == std::string::npos) {
			pos = body.find("\n");
			pos += 1;
		} else {
			pos += 2;
		}

		body = body.substr(pos);

		if (body.size() != 0)
			getChunkedData(body);
		else
			return ;
	}
}