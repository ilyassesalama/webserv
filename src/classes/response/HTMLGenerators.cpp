#include "../../../webserv.hpp"

std::string generateHTMLStart(std::string path) {
    return "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>Document</title><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\"crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Jost\"rel=\"stylesheet\"><style>html,body,p,ol,ul,li,dl,dt,dd,blockquote,figure,fieldset,legend,textarea,pre,iframe,hr,h1,h2,h3,h4,h5,h6{margin:0;padding:0}h1,h2,h3,h4,h5,h6{font-size:100%;font-weight:normal}ul{list-style:none}button,input,select{margin:0}html{box-sizing:border-box; font-family: \"Jost\", sans-serif;}*,*::before,*::after{box-sizing:inherit}img,video{height:auto;max-width:100%}iframe{border:0}table{border-collapse:collapse;border-spacing:0}td,th{padding:0}.auto_index_template {padding: 40px;width: 100%;}.path {font-size: 28px;}.line {width: 100%;height: 2px;background-color: #333;margin: 20px 0;}.files {display: flex;flex-direction: column;}.file {display: grid;grid-template-columns: repeat(3, 1fr);justify-content: space-between;}.header {font-weight: bold;margin-bottom: 10px;}.name, .update, .size {text-align: left;}</style></head><body><div class=\"auto_index_template\"><h1 class=\"path\">Index of " + path + "</h1><div class=\"line\"></div><div class=\"files\"><div class=\"file header\"><div class=\"name\">Name</div><div class=\"update\">Creation Date</div><div class=\"size\">Size</div></div>";
}

std::string createAnchor(std::string name) {
	return "<a href=\"" + name + "\" class=\"name\">" + name + "</a>";
}

std::string createParagraph(std::string lastUpdate) {
	return "<p class=\"update\">" + lastUpdate + "</p>";
}

std::string createSizeParagraph(long long size) {
	return "<p class=\"size\">" + String::to_string(size) + " bytes</p>";
}

std::string createBodyHTML(std::string nameHTML, std::string lastUpdateHTML, std::string sizeHTML) {
    return "<div class=\"file\">" + nameHTML + lastUpdateHTML + sizeHTML + "</div>";
}

std::string generateHTMLEnd(std::string bodyHTML) {
	return bodyHTML + "</div><div class=\"line\"></div></div></body></html>";
}

void Response::autoIndexHTMLBuilder(size_t slashPos) {

	std::string filePath;
	std::string bodyHTML;
	std::string autoIndexHTML;
	std::time_t lastUpdateTime;

	DIR* directory = opendir(this->path.c_str());

	autoIndexHTML = generateHTMLStart(this->request->getRequestLine()["path"]);

	if (directory) {

		struct dirent* entry;

		while ((entry = readdir(directory)) != NULL) {

			if (entry->d_name[0] == '.')
				continue ;

			filePath = this->path;
			filePath += slashPos != this->path.size() - 1 ? "/" : "";
			filePath += entry->d_name;

			struct stat stats;

			if (stat(filePath.c_str(), &stats) == 0) {
				lastUpdateTime = stats.st_mtime;
			} else {
				this->statusCode = 500;
				this->responseBody = this->getErrorPageHTML();
				return ;
			}

			bodyHTML += createBodyHTML(createAnchor(entry->d_name), createParagraph(std::ctime(&lastUpdateTime)), createSizeParagraph(static_cast<long long>(stats.st_size)));
		}
		closedir(directory);
	} else {
		this->statusCode = 500;
		this->responseBody = this->getErrorPageHTML(); 
		return ;
	}

	autoIndexHTML += generateHTMLEnd(bodyHTML);

	this->responseBody = autoIndexHTML;
	this->statusCode = 200;
	std::fstream myFile("/tmp/index.html", std::ios::out);
	myFile << this->responseBody;
	this->path = "/tmp/index.html";
	myFile.close();
}