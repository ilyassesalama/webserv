#include "../../../webserv.hpp"

// void uploadSingleFile(std::map<std::string, std::string> headers, std::string &fileName, t_route *currentRoute) {

// 	std::fstream bufferFile(fileName, std::ios::binary | std::ios::app);
// 	if (!bufferFile.is_open()) {
// 		throw Utils::WebservException("Error opening file");
// 	}

// 	std::fstream myFile(File::getWorkingDir() + currentRoute->upload_path + File::generateFileName("uploaded") + File::getExtension(headers), std::ios::binary | std::ios::app);
// 	if (!myFile.is_open()) {
// 		throw Utils::WebservException("Error opening file");
// 	}
// 	myFile << bufferFile.rdbuf();

// 	myFile.close();
// 	bufferFile.close();
// }

void Response::POSTResponseBuilder() {

	if (this->statusCode == 200) {
		this->isCGI = this->isLocationHasCGI();

		if (this->isCGI || !this->currentRoute->upload) {
			this->setResponseBody();
		} else {
			this->statusCode = 201;
			if (this->request->getIsRequestMultipart())
				std::cout << "will enter if header is multipart" << std::endl;
			else if (!this->request->getIsRequestChunked())
				std::cout << "will enter if header is content-length" << std::endl;

			this->responseBody = "";
		}
	}

	
    this->setHeaders();
    this->setResponseLine();
	this->fileOffset = -2;
	addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}