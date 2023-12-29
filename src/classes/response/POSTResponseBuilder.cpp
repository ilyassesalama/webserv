#include "../../../webserv.hpp"

void uploadSingleFile(std::map<std::string, std::string> headers, std::string &fileName, std::string body) {
	fileName = File::generateFileName("uploaded") + File::getExtension(headers);
	std::fstream myFile(File::getWorkingDir() + "/uploads/" + fileName, std::ios::binary | std::ios::app);
	if (!myFile.is_open()) {
		throw std::runtime_error("Error opening file");
	}
	myFile << body;
}

void Response::POSTResponseBuilder() {
	this->isCGI = this->isLocationHasCGI();
	if (!this->isCGI) {
		uploadSingleFile(this->request->getHeaders(), this->request->getFileName(), this->request->getBody());
	}
	this->responseBody = "";
    this->setHeaders();
    this->setResponseLine();
	this->fileOffset = -2;
	addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}