#include "../Headers/WebServerIncludes.hpp"

std::string	generateFilename() {
	std::string	filename("/tmp/cgi_output");
	while (open(filename.c_str(), O_RDWR, 0644))
		filename += "t";
	return filename;
}

std::string	httpServer::generateDirectoryListing(std::vector<std::string> dirContent, pars& p, std::string root) {
	std::string path;
    std::string htmlContent("<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>");
	for (size_t i = 0; i < dirContent.size(); ++i) {
        if (dirContent[i] != "." && dirContent[i] != "..") {
			if (CheckDirectory(dirContent[i]))
				generateDirectoryListing(getDirContent(dirContent[i]), p, root + "/" + dirContent[i]);
			path = root + "/" + dirContent[i];
			erase_consequtive_slash(path);
			erase_consequtive_slash(path);
           	htmlContent += "<li><a href=\"" + path + "\">" + dirContent[i] + "</a></li>";
        }
	}
    htmlContent += "</ul></body></html>";
	p.ex = ".html";
	p.to_write = htmlContent.size();
	return htmlContent;
}

bool isSocketOpen(int socket_fd) {
    int error = 0;
    socklen_t len = sizeof(error);

    if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len) != 0) {
        std::cerr << "Error in getsockopt()" << std::endl;
        return false;
    }
    return (error == 0);
}

void	httpServer::generateResponse(std::string body, clientSocket& client) {
	std::stringstream	response;
	int	header_size = 0;
	ssize_t bytesSent = 0;
	size_t totalBytesSent = 0;
	
	if (!client.p.status)
		client.p.status = 200;
		
	if (!client.p.written) {
		response << "HTTP/1.1 " << status_phrase[client.p.status] << "\r\nContent-Type: ";
		response << mime_type[client.p.ex.c_str()] << "; charset=UTF-8";
		if (!client.p.file_name.empty() && (client.p.status == 301 || client.p.status == 307)) // this was the reason
			response << "\r\nlocation: http://" << client.p.headers["Host"] << client.p.file_name;
		if (client.p.ex == ".php") {
			if (client.p.headers.find("Date") != client.p.headers.end())
				response << "\r\nDate: " << client.p.headers["Date"];
			if (client.p.headers.find("Set-Cookie") != client.p.headers.end())
				response << "\r\nSet-Cookie: " << client.p.headers["Set-Cookie"];
			if (client.p.headers.find("Cache-Control") != client.p.headers.end())
				response << "\r\nCache-Control: " << client.p.headers["Cache-Control"];
			if (client.p.headers.find("Expires") != client.p.headers.end())
				response << "\r\nExpires: " << client.p.headers["Expires"];
		}
		response << "\r\nContent-Length: " << client.p.to_write << "\r\n\r\n";
		header_size = response.str().size();
	}
	if (!body.empty())
		response << body;
	size_t size = (!client.p.written || response.str().size() < CHUNK_SIZE)? response.str().size() : CHUNK_SIZE;
	if ((bytesSent = send(client.socket_fd, response.str().c_str(), size, O_NONBLOCK)) < 0) {
		if (client.p.fdout > 0)
			close(client.p.fdout);
		client.p.end_flag = 0;
		return ;
	}
	else {
		client.p.written += (bytesSent - header_size);
		client.p.chyata.clear();
		totalBytesSent += bytesSent;
		if (totalBytesSent < size) {
			client.p.chyata = response.str().substr(totalBytesSent, response.str().length());
		}	
	}
	if (client.p.written >= client.p.to_write || body.empty()) {
		if (client.p.fdout > 0)
			close(client.p.fdout);
		client.p.end_flag = 0;
	}
}

std::string	httpServer::generateDynamicPage(int status, pars& p) {
	std::stringstream htmlContent;
	p.ex = ".html";
	htmlContent << "<html><head><title>Dynamic Page</title></head><body><h1>" << status_phrase[status] << "</h1></body></html>";
	p.to_write = htmlContent.str().size();
	return htmlContent.str();
}