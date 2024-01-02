#include "../../../webserv.hpp"

std::string getBoundaryFileName(std::string boundaryContent, std::string contentType) {
	size_t startPosition = boundaryContent.find("filename=\"");
	if (startPosition != std::string::npos)
		startPosition += 10;
	else {
		startPosition = boundaryContent.find("name=\"");
		if (startPosition != std::string::npos)
			startPosition += 6;
	}

	size_t endPosition = boundaryContent.find("\"", startPosition);
	return startPosition != std::string::npos && !boundaryContent.substr(startPosition, endPosition - startPosition).empty() ? boundaryContent.substr(startPosition, endPosition - startPosition) : File::generateFileName("boundary") + File::getContentTypeExtension(contentType);
}

void Response::handleboundaryStart(std::ifstream& inputfile) {
	std::string boundaryContent;
	std::string contentType = "";
	std::string line;
	std::map<std::string, std::string> contentTempMap;

	while(std::getline(inputfile,line)) {
		if(line == "\r") break;

		if(line.find("Content-Type: ") != std::string::npos) {
			contentType = line.substr(14, line.substr(14).find("\r"));
			std::cout<< "Content-Type: " << contentType << std::endl;
		}
		boundaryContent.append(line+'\n');
	}
	
	std::string BoundaryFileName = getBoundaryFileName(boundaryContent, contentType);

	this->uploadFilePath = File::getWorkingDir() + this->currentRoute->upload_path + BoundaryFileName;
}
void Response::saveOnFile(std::string data) {
    std::ofstream outputFile(this->uploadFilePath.c_str(), std::ios::binary | std::ios::app);

    if (!outputFile.is_open()) {
		this->statusCode = 500;
		buildErrorResponse();
        throw Utils::WebservException("Error opening the output file");
    } else {
        outputFile << data + '\n';
        outputFile.close();
    }
}

void Response::uploadBoundaryFile() {
	std::ifstream inputFile(this->tmpUploadFilePath.c_str(), std::ios::binary);
	std::string line;
	int count = 100;
	if(!inputFile.is_open()) {
		this->statusCode = 500;
		this->responseVector.clear();
		buildErrorResponse();
		throw Utils::WebservException("Error opening the input file");
	}
	inputFile.seekg(this->uploadFileOffset);
	while(std::getline(inputFile,line)) {
		if(line == "--" + this->boundary + '\r') {
			handleboundaryStart(inputFile);
		} else if(line == "--" + this->boundary + "--\r") {
			this->uploading = false;
			File::removeFile(this->request->getFileName());
			break; 
		}
		  else saveOnFile(line);
		count --;
		if(count == 0) break;
	}
	this->uploadFileOffset = inputFile.tellg();
	if(this->uploadFileOffset == -1) {
		this->uploading = false;
		setServingState(false);
		File::removeFile(this->request->getFileName());
	}
}

void Response::POSTResponseBuilder() {
	if(isCGIon()) {
		if(File::isFile(this->path)) {
			this->handleFileRequest();
			File::removeFile(this->request->getFileName());
		}
		else if(File::isDirectory(this->path)) {
			size_t slashPos = this->path.find_last_of("/");
			std::string index = slashPos != this->path.size() - 1 ? "/" : "";
			index.append(this->currentRoute->index);
			std::cout << index << std::endl;
			if(File::isFile(this->path + index)) {
				this->path.append(index);
				this->handleFileRequest();
				File::removeFile(this->request->getFileName());
			}
			else {
				this->statusCode = 404;
				throw(Utils::WebservException("File Not Found ..."));
			}
		}
	}
	else if(this->currentRoute->upload) {
		this->statusCode = 201;
		this->uploading = true;
		this->responseBody = "";
		this->tmpUploadFilePath = this->request->getBoundaryInfos(1);

		if (this->request->getIsRequestMultipart()) {
			this->boundary = this->request->getBoundaryInfos(0);
		}
		else this->uploading = false;
	}
	else {
		this->statusCode = 403;
		throw(Utils::WebservException("ResponseBuilder: 403 Forbidden"));
	}

	this->setHeaders();
    this->setResponseLine();
	this->fileOffset = -2;
	addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}