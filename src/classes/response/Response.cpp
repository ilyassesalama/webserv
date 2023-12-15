#include "../../../webserv.hpp"

Response::Response() {
    (*this).clientSidePath = "/Users/abahsine/Documents/1337-projects/webserv/src/client-side/";
    (*this).path = "";
    (*this).server = NULL;
    (*this).request = NULL;
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
	(*this).isServed = false;
	(*this).servedBytes = 0;
}

Response::~Response() {

}

int Response::getServerdBytes() {
	return((*this).servedBytes);
}

bool Response::getServingStatus() {
	return((*this).isServed);
}

void Response::setServedBytes(int bytes) {
	(*this).servedBytes +=  bytes;
	if((*this).servedBytes == (int)(*this).response.length())
		(*this).isServed = true;
}

std::string Response::getResponse() {
	return((*this).response);
}

std::string Response::getErrorPageHTML(){

    std::string responseBody;

    switch(this->statusCode){
        case 404:
            this->path = "src/client-side/error_pages/404.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 500:
            this->path = "src/client-side/error_pages/500.html";
            responseBody = File::getFileContent(this->path);
            break;
        default:
            this->path = "src/client-side/error_pages/501.html";
            responseBody = File::getFileContent(this->path);
            break;
    }
    return responseBody;
}


std::string Response::getStringStatus(){
    switch(this->statusCode){
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

void Response::handleDirectoryRequest() {

    size_t slashPos = this->path.find_last_of("/");

    std::string indexHTML = slashPos != this->path.size() - 1 ? "/index.html" : "index.html";

    if (File::isFile(this->path + indexHTML)) {
		this->path += indexHTML;
        this->responseBody = File::getFileContent(this->path);
		this->statusCode = 200;
    } else {
        bool directory_listing = true; // temporary

        if (directory_listing) {
            // response should be list of files in the directory
			this->autoIndexHTMLBuilder(indexHTML);
        } else {
            this->statusCode = 403;
            this->responseBody = this->getErrorPageHTML();
        }
    }
}

void Response::handleFileRequest() {

    std::vector<std::string>cgi_methods; // temporary
    // cgi_methods.push_back("GET"); // temporary
    std::string cgi_extension = "php"; // temporary

    if (cgi_methods.size() != 0) {

        if (!cgi_extension.empty() && std::find(cgi_methods.begin(), cgi_methods.end(), "GET") != cgi_methods.end()) {

        }

    } else {
        this->responseBody = File::getFileContent(this->path);
		this->statusCode = 200;
    }

}

void Response::setResponseLine() {

    std::string status = getStringStatus();

    this->responseLine = "HTTP/1.1 " + status;
}

void Response::setHeaders() {

	std::string contentType;
	std::string contentSize;

	contentType = File::getContentType(this->path);
	(*this).contentLength = this->responseBody.size();
	contentSize = String::to_string((*this).contentLength);
	this->responseHeaders = "\nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n";
}

void Response::setResponseBody() {

	// Under construction
	std::vector<std::string> allowed_methods;
	allowed_methods.push_back("POST");

	bool uploadSupport = true;

	if (allowed_methods.size() != 0 && std::find(allowed_methods.begin(), allowed_methods.end(), "POST") != allowed_methods.end() && this->request->getRequestLine()["method"] == "POST" && uploadSupport) {
		
	} else if (File::isDirectory(this->path)) {
        this->handleDirectoryRequest();
    } else if (File::isFile(this->path)) {
        this->handleFileRequest();
    } else {
        this->statusCode = 404;
        this->responseBody = this->getErrorPageHTML();
    }
}

void Response::setTroute() {
    (*this).routes = server->routes;
}

std::string getPathLocation(std::string path) {
    std::string location;

    size_t firstSlash = path.find("/");

    if(firstSlash != std::string::npos) {
        size_t secondSlash = path.find("/", firstSlash + 1);
        if(secondSlash != std::string::npos) {
             return path.substr(0, secondSlash); 
        }
    }
    return("/");
}

t_route *Response::getSpecificRoute(std::string location) {
    for(std::vector<t_route>::iterator it = (*this).routes.begin(); it != (*this).routes.end(); ++it) {
        if(location == it->path) {
            return(&(*it));
        }
    }
    return(NULL);
}

void Response::buildResourcePath(t_route *route) {
    std::string requestedResource;
    requestedResource.append(this->clientSidePath);
    requestedResource.append(route->root);
    requestedResource.append(this->path);
    (*this).path = requestedResource;
}

void Response::setPath(std::string path) {
    (*this).path = path;
    std::string locationPath = getPathLocation(path);

    t_route *route = getSpecificRoute(locationPath);
    if(route == NULL) {
        if(locationPath == "/") {
            //resource not found set error code 
            std::cout << "NO LOCATION FOUND" << std::endl;
        }
        else  buildResourcePath(getSpecificRoute("/")); 
    }
    else buildResourcePath(route);

}

void Response::clearResponse() {

    (*this).path = "";
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
	(*this).clientSidePath = "/Users/abahsine/Documents/1337-projects/webserv/src/client-side";
}

void Response::setRequest(RequestParser &request) {
    (*this).request = &request;
}

void Response::setServer(t_server &server) {
    (*this).server = &server;
    setTroute();
}

void Response::responseBuilder() {
    if (this->request->getRequestLine()["method"] == "GET") {
        this->setPath(request->getRequestLine()["path"]);
		Log::w(path);
        this->GETResponseBuilder();
        
    } else if (this->request->getRequestLine()["method"] == "POST") {
		this->setPath(request->getRequestLine()["path"]);
		Log::w(path);
		this->POSTResponseBuilder();
    } else if (this->request->getRequestLine()["method"] == "DELETE") {

    }
}

// Response::Response(int clientFd, const RequestParser &parser){
//     this->clientFD = clientFd;
//     this->requestParser = parser;
//     std::string path = requestParser.getRequestLine()["path"];
//     std::string finalPath = "src/client-side";

// 	std::cout << path << std::endl;

// 	if (File::isDirectory(finalPath + path)) {

// 		std::string indexHTML = path.find_last_of("/") != std::string::npos && path.find_last_of("/") == path.size() -1 ? "index.html" : "/index.html";

// 		if (File::isFile(finalPath + path + indexHTML)) {
// 			finalPath += path + indexHTML;
// 		} else {
// 			bool directory_listing = true; // This is a temporary variable, we should use value form config file
// 			if (directory_listing) {
// 				Log::i("heyyyyyyy");
// 				// autoindex here
// 				finalPath += "/auto_index_template.html";

// 				// handleAutoIndex(finalPath + path);
// 			} else {
// 				this->status = 403;
// 				Log::e("Response: 403 Forbidden");
// 				return;
// 			}
// 		}

// 	} else if (File::isFile(finalPath + path)) {
// 		finalPath += path;
// 	} else {
// 		this->status = 403;
// 		Log::e("Response: 403 Forbidden");
// 		Log::e("path: " + finalPath + path);
// 		return;
// 	}

//     Log::i("Fetching response from \"" + finalPath + "\"...");
//     try {
//         std::string content = File::getFileContent(finalPath);
//         this->response = content;
//         this->status = 200;
//     } catch (std::exception &e) {
//         this->status = 404;
//         Log::e("Response: 404 Not Found");
//     }
//     this->path = finalPath;
// }

// std::string Response::sendResponse() {
//     if(this->status != 200){ // TOFIX: THIS WILL MAKE OTHER FILES SUCH AS CSS AND JS TO BE SENT AS HTML. FIX THIS!
//         this->getErrorPageHTML();
//     }
//     this->contentType = File::getContentType(path);
//     this->contentSize = std::to_string(this->response.length());
//     std::string status = getStringStatus();

//     response = "HTTP/1.1 " + status + " \nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n" + response;
//     if(send(clientFD, response.c_str(), response.length(), 0) == -1)
//         std::cout << "awiliiii" << std::endl;
//     // this->contentType.clear();
//     // this->contentSize.clear();
//     // response.clear();
//     // path.clear();
//     Log::i("Response sent");
//     Log::d("Response information:\n- Content-Type: " + contentType + "\n- Content-Length: " + contentSize + "\n- Status: " + status + "\n- Path: " + path);
//     return(response);
// }

// std::string Response::getErrorPageHTML(){
//     switch(status){
//         case 404:
//             this->path = "src/client-side/error_pages/404.html";
//             this->response = File::getFileContent(this->path);
//             break;
//         case 500:
//             this->path = "src/client-side/error_pages/500.html";
//             this->response = File::getFileContent(this->path);
//             break;
//         default:
//             this->path = "src/client-side/error_pages/501.html";
//             this->response = File::getFileContent(this->path);
//             break;
//     }
//     return this->response;
// }

// void Response::setPath(const std::string &path){
//     this->path = path;
// }

// const std::string &Response::getResponse(){
//     return this->response;
// }

// const int &Response::getStatus(){
//     return this->status;
// }

// std::string Response::getStringStatus(){
//     switch(this->status){
//         case 200:
//             return "200 OK";
//         case 404:
//             return "404 Not Found";
//         case 500:
//             return "500 Internal Server Error";
//         default:
//             return "501 Unknown";
//     }
// }