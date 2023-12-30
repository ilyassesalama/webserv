#include "../../../webserv.hpp"

void RequestParser::escapeCRLF(std::string &body) {
	
	if (this->previousChunkCRLF.empty()) {
		int sum = body.size() > 1 ? 2 : 1;

		body = body.substr(sum);

		this->previousChunkCRLF = sum == 1 ? "\r" : "";

		this->isCRLF = sum == 2 ? false : true;
	} else {
		body = body.substr(1);
		this->previousChunkCRLF = "";
		this->isCRLF = false;
	}
}

size_t chunkToDecimal(std::string &sizeBuffer, size_t pos) {
	std::string firstLine = sizeBuffer.substr(0, pos);
	size_t hexSize;
	std::stringstream buffer(firstLine);
	buffer >> std::hex >> hexSize;
	sizeBuffer = "";
	return hexSize;
}

/*
	this function subtracts the size of the next chunk from the request body
	the main function only calls this when chunk remainder is zero
*/
size_t RequestParser::getChunkSize(std::string &body) {


	// create a boolean so that it does not enter first request
	if (this->isCRLF && this->previousChunkCRLF.find("\n") == std::string::npos)
		escapeCRLF(body);

	if (body.empty())
		return std::string::npos;

	size_t pos = body.find("\n");
	pos = pos == std::string::npos ? body.size() : pos + 1;

	this->sizeBuffer += body.substr(0, pos);

	pos = this->sizeBuffer.rfind("\n");

	if (pos != std::string::npos)
		return chunkToDecimal(this->sizeBuffer, pos);

	return std::string::npos;
}

void RequestParser::openFile(std::fstream &myFile) {
	myFile.open(File::getWorkingDir() + this->route->upload_path + this->fileName, std::ios::binary | std::ios::app);
	if (!myFile.is_open()) {
		this->parsingState.ok = true;
		this->parsingState.statusCode = 500;
		throw Utils::WebservException("Error opening file");
	}
}
/*
	this is the main function to get chunks and it works using recursion
*/
void RequestParser::getChunkedData(std::string &body) {

	std::fstream myFile;

	if (this->chunkRemainder == 0 || this->chunkRemainder == std::string::npos) {
		this->chunkRemainder = getChunkSize(body);

		if (this->chunkRemainder == 0) { // maybe this block is useless now
			openFile(myFile);
			this->parsingState.bodyOk = true;
			myFile.close();
			return ;
		}
		if (this->chunkRemainder != std::string::npos) {
			size_t pos = body.find("\n"); // removing chunk size from body
			body = body.substr(++pos);
		}
	}

	if (this->chunkRemainder != std::string::npos) {

		size_t bodyEnd;
		openFile(myFile);

		if (this->chunkRemainder > body.size())
			bodyEnd = body.size();
		else
			bodyEnd = this->chunkRemainder;

		std::string chunkStr = body.substr(0, bodyEnd);
		body = body.substr(bodyEnd);

		myFile << chunkStr;

		this->chunkRemainder -= chunkStr.size();

		if (this->chunkRemainder == 0)
			this->isCRLF = true;

		myFile.close();

		if (body.size() != 0) 
			getChunkedData(body);
	}
}