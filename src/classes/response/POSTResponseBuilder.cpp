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

std::string generateRandomName() {
    // Characters to use in the random name
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // Length of the random name
    const int nameLength = 10;

    // Generate the random name
    std::string randomName;
    for (int i = 0; i < nameLength; ++i) {
        int randomIndex = std::rand() % characters.length();
        randomName += characters[randomIndex];
    }

    return randomName;
}

void Response::handleboundaryStart(std::ifstream& inputfile) {
	std::string boundaryContente;
	std::string Content_Type = "";
	std::string line;
	std::map<std::string, std::string> contentTempMap;

	while(std::getline(inputfile,line)) {
		if(line == "\r")
			break;
		if(line.find("Content-Type: ") != std::string::npos) {
			std::string contentType = line.substr(14, line.substr(14).find("\r"));
			Log::w("<" + contentType + ">");
			contentTempMap["Content-Type"] = contentType;
			Log::w(File::getExtension(contentTempMap));

		}
		boundaryContente.append(line+'\n');
	}

	
	this->uploadFilePath = File::getWorkingDir() + "/uploads/" + generateRandomName() + File::getExtension(contentTempMap);
	std::cout << this->uploadFilePath << std::endl;
}
void Response::saveOnFile(std::string data) {
    std::ofstream outputFile(this->uploadFilePath.c_str(), std::ios::binary | std::ios::app);

    if (!outputFile.is_open()) {
        Log::e("Error opening the output file");
        throw std::runtime_error("Error opening the output file");
    } else {
        outputFile << data + '\n';
        outputFile.close();  // Close the file explicitly
    }
}



void Response::uploadFile() {
	std::ifstream inputFile(this->boundaryFilePath.c_str(), std::ios::binary);
	std::string line;
	int count = 100;
	if(!inputFile.is_open()) {
		Log::e("error opening the file ");
		//need to be changed
		exit(0);
	}
	inputFile.seekg((*this).uploadFileOffset);
	while(std::getline(inputFile,line)) {
		if(line == "--" + this->boundary + '\r') {
			Log::e("handle boundary ...");
			handleboundaryStart(inputFile);
		}
		else if(line == "--" + this->boundary + "--") {
			Log::e("upload finished ...");
			//need to be changed just for testing 
			exit(0);
		}
		else
			saveOnFile(line);
		count --;
		if(count == 0) {
			std::cout << "break;" << std::endl;
			break;
		}
	}
	this->uploadFileOffset = inputFile.tellg();
	if(this->uploadFileOffset == -1) {
		this->uploading = false;
		setServingState(false);
	}
}

void Response::POSTResponseBuilder() {

	if (this->statusCode == 200) {
		this->isCGI = this->isLocationHasCGI();

		if (this->isCGI || !this->currentRoute->upload) {
			this->setResponseBody();
		} else {
			this->statusCode = 201;
			if (this->request->getIsRequestMultipart()) {
				//note !!!!! : this condition did not triggered even the request is multipart !!!!!
				std::cout << "will enter if header is multipart" << std::endl;
				this->uploading = true;
				this->boundary = this->request->getBoundaryInfos(0);
				this->boundaryFilePath = "/tmp/" + this->request->getBoundaryInfos(1);
				this->statusCode = 201;
				this->responseBody = "";

			}
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