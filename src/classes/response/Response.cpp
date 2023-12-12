#include "../../../webserv.hpp"

Response::Response(int clientFd, const RequestParser &parser){
    this->clientFD = clientFd;
    this->requestParser = parser;
    std::string path = requestParser.getRequestLine()["path"];
    std::string finalPath = "src/client-side";

	size_t slashPos = path.find_last_of("/");
	size_t pointPos = path.find_last_of(".");
	if (slashPos == std::string::npos) {
		this->status = 404;
        Log::e("Response: 404 Not Found");
        return;
	}

    if(path == "/" || pointPos == std::string::npos || slashPos > pointPos) { // If path is not a file enter here

        finalPath += "/main" + path + "index.html";
		
		Log::d("finalPath: " + finalPath);

		if (slashPos != path.size() - 1) { // If URI does not have "/" at end enter here
			this->status = 404;
			Log::e("Response: 301 Moved Permanently");
			return;
		}
		
		if (access(finalPath.c_str(), F_OK) == -1) { // If index.html not found enter here

			bool directory_listing = false; // This is a temporary variable, we should use value form config file
			if (directory_listing) {
				// autoindex here
			} else {
				this->status = 403;
				Log::e("Response: 403 Forbidden");
				return;
			}

		}

	} else { // If path is a file enter here
        finalPath += "/main" + path;
	}


    Log::i("Fetching response from \"" + finalPath + "\"...");
    try {
        std::string content = File::getFileContent(finalPath);
        this->response = content;
        this->status = 200;
    } catch (std::exception &e) {
        this->status = 404;
        Log::e("Response: 404 Not Found");
    }
    this->path = finalPath;
}

void Response::sendResponse() {
    if(this->status != 200){ // TOFIX: THIS WILL MAKE OTHER FILES SUCH AS CSS AND JS TO BE SENT AS HTML. FIX THIS!
        this->getErrorPageHTML();
    }
    this->contentType = File::getContentType(path);
    this->contentSize = std::to_string(this->response.length());
    std::string status = getStringStatus();

    response = "HTTP/1.1 " + status + " \nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n" + response;
    send(clientFD, response.c_str(), response.length(), 0);
    Log::i("Response sent");
    Log::d("Response information:\n- Content-Type: " + contentType + "\n- Content-Length: " + contentSize + "\n- Status: " + status + "\n- Path: " + path);
}

std::string Response::getErrorPageHTML(){
    switch(status){
        case 404:
            this->path = "src/client-side/error_pages/404.html";
            this->response = File::getFileContent(this->path);
            break;
        case 500:
            this->path = "src/client-side/error_pages/500.html";
            this->response = File::getFileContent(this->path);
            break;
        default:
            this->path = "src/client-side/error_pages/501.html";
            this->response = File::getFileContent(this->path);
            break;
    }
    return this->response;
}

void Response::setPath(const std::string &path){
    this->path = path;
}

const std::string &Response::getResponse(){
    return this->response;
}

const int &Response::getStatus(){
    return this->status;
}

std::string Response::getStringStatus(){
    switch(this->status){
        case 200:
            return "200 OK";
        case 404:
            return "404 Not Found";
        case 500:
            return "500 Internal Server Error";
        default:
            return "501 Unknown";
    }
}