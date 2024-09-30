#include "../Headers/WebServerIncludes.hpp"

void	httpServer::setServerMatrix(std::vector<ServerConfig> _servers, std::vector<std::pair<std::string, int> > serverHosts) {
	std::vector<std::pair<std::string, int> >::iterator it;
	size_t	i = 0;

	for (it = serverHosts.begin(); it != serverHosts.end(); ++it) {
		for (size_t j = 0; j < _servers.size(); ++j) {
			if (_servers[j].GetHost() == it->first && _servers[j].GetPort() == it->second)
				data[i].push_back(_servers[j]);
		}
		i++;
	}
	// for (it = serverHosts.begin(); it != serverHosts.end(); ++it) {
	// 	std::cout << "Server: " << it->first << ":" << it->second << std::endl;
	// }
	// for (size_t i = 0; i < serverHosts.size(); ++i) {
	// 	std::cout << "server " << i << " has " << data[i].size() << " servers" << std::endl;
	// }
}

void	httpServer::setStatusPhrase(void) {
	status_phrase[100] = "100 Continue";
	status_phrase[200] = "200 OK";
	status_phrase[201] = "201 Created";
	status_phrase[202] = "202 Accepted";
	status_phrase[204] = "204 No Content";
    status_phrase[302] = "302 Found";
	status_phrase[301] = "301 Moved Permanently";
	status_phrase[307] = "307 Temporary Redirect";
	status_phrase[400] = "400 Bad Request";
	status_phrase[401] = "401 Unauthorized";
	status_phrase[403] = "403 Forbidden";
	status_phrase[404] = "404 Not Found";
	status_phrase[405] = "405 Method Not Allowed";
	status_phrase[409] = "409 Conflict";
	status_phrase[411] = "411 Length Required";
	status_phrase[413] = "413 Request Entity Too Large";
	status_phrase[414] = "414 Request-URI Too Large";
	status_phrase[415] = "415 Unsupported Media Type";
	status_phrase[500] = "500 Internal Server Error";
	status_phrase[501] = "501 Not Implemented";
	status_phrase[502] = "502 Bad Gateway";
	status_phrase[503] = "503 Service Unavailable";
	status_phrase[504] = "504 Gateway Time-out";
	status_phrase[505] = "505 HTTP Version not supported";
}

void	httpServer::setMimeType(void) {
	mime_type[".html"] = "text/html";
	mime_type[".php"] = "text/html";
	mime_type[".py"] = "text/html";
    mime_type[".css"] = "text/css";
	mime_type[".txt"] = "text/plain";
    mime_type[".csv"] = "text/csv";
    mime_type[".rtf"] = "text/rtf";
    mime_type[".js"] = "application/javascript";
    mime_type[".json"] = "application/json";
    mime_type[".xml"] = "application/xml";
    mime_type[".pdf"] = "application/pdf";
    mime_type[".doc"] = "application/msword";
    mime_type[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mime_type[".xls"] = "application/vnd.ms-excel";
    mime_type[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mime_type[".ppt"] = "application/vnd.ms-powerpoint";
    mime_type[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mime_type[".zip"] = "application/zip";
    mime_type[".tar"] = "application/x-tar";
    mime_type[".gz"] = "application/gzip";
	mime_type[".mp3"] = "audio/mpeg";
    mime_type[".wav"] = "audio/wav";
    mime_type[".ogg"] = "audio/ogg";
    mime_type[".mp4"] = "video/mp4";
    mime_type[".avi"] = "video/x-msvideo";
    mime_type[".mpeg"] = "video/mpeg";
    mime_type[".bmp"] = "image/bmp";
    mime_type[".ico"] = "image/x-icon";
    mime_type[".svg"] = "image/svg+xml";
    mime_type[".gif"] = "image/gif";
    mime_type[".png"] = "image/png";
    mime_type[".jpg"] = "image/jpeg";
    mime_type[".jpeg"] = "image/jpeg";
    mime_type[".tiff"] = "image/tiff";
}

void	httpServer::setExtension(std::string filename, pars& p) {
	size_t i = filename.find_last_of(".");
	if (i == std::string::npos)
		p.ex =  std::string(".txt");
	else
		p.ex =  filename.substr(i);
	if (mime_type.find(p.ex) == mime_type.end())
		p.status = 415;
}