#include "../Headers/WebServerIncludes.hpp"

ServerConfig	httpServer::getMatchingServer(pars& p, std::vector<ServerConfig>& data) {
	
	for (size_t i = 0; i < data.size(); ++i) {
		if (data[i].GetServerNames() == p.headers["Host"])
			return data[i];		
	}
	for (size_t i = 0; i < data.size(); ++i) {
		if (data[i].GetDefaultServer())
			return data[i];		
	}
	return data[0];
}

Location	httpServer::getMatchingLocation(pars &p, ServerConfig& data) {
	size_t i;
	std::vector<Location> location = data.GetLocation();
	std::string url = p.headers["url"];
	std::string method = p.headers["method"];
	sort_location(location);
	for (i = 0; i < location.size(); ++i) {
		std::string path = location[i].GetPath();
    	if (url.find(path) == 0) {
			if (!url.substr(path.length(), url.length()).empty() && url.substr(path.length(), url.length())[0] != '/')
				continue ;
        	std::vector<std::string> allowedMethods = location[i].GetMethods();			
			if (allowedMethods.size() != 0) {
				if (std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end())
					return location[i];
				else {
					p.status = 405;
					return Location();
				}
			}
			else
				return location[i];
    	}
	}
	p.status = 404;
	return Location();
}

std::string	httpServer::getRoot(ServerConfig data, Location& matchedLocation) {
	std::vector<Location> location = data.GetLocation();
	sort_location(location);
	if (!matchedLocation.GetPath().empty() && !matchedLocation.GetRoot().empty())
		return (matchedLocation.GetRoot());
	if (location[location.size() - 1].GetPath() ==  "/" && !location[location.size() - 1].GetRoot().empty())
		return (location[location.size() - 1].GetRoot());
	if (!data.GetRoot().empty())
		return (data.GetRoot());
	return std::string();
}

std::vector<std::string> httpServer::getDirContent(std::string& path) {
	std::vector<std::string> dirContent;
	DIR* dir = opendir(path.c_str());
	if (dir == NULL) {
		return dirContent;
	}
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		dirContent.push_back(entry->d_name);
	}
	closedir(dir);
	return dirContent;
}

std::string	httpServer::getPath(clientSocket& client, ServerConfig& data, Location& matchedLocation) {
	std::string path;
	std::string	root = getRoot(data, matchedLocation);
	std::string url = client.p.headers["url"]; 
	std::string method = client.p.headers["method"];
	if (root.empty()) {
		client.p.status = 500;
		return std::string();
	}
	if (!matchedLocation.GetPath().empty())
		path = root + url.substr(matchedLocation.GetPath().length());
	else {
		client.p.status = 404;
		return std::string();
	}
    if (url == "/")
		path = root;
	if (!matchedLocation.GetRedirect().empty()) {
		client.p.status = 301; // or 302
		client.p.file_name = matchedLocation.GetRedirect() + "/" + url.substr(matchedLocation.GetPath().length(), url.length());
		return path;
	}
	std::vector<std::string> dirContent = getDirContent(path);
	erase_consequtive_slash(path);
	if (dirContent.size() != 0) {
			if (!matchedLocation.GetDefaultFile().empty()) {
				if (std::find(dirContent.begin(), dirContent.end(),  matchedLocation.GetDefaultFile()) != dirContent.end()) {
					if (path[path.length() - 1] != '/')
						path += "/";
					return path + matchedLocation.GetDefaultFile();
				}
			}
			if (matchedLocation.GetDirectoryListing())
				return path;
			else if (method != "DELETE") {
				client.p.status = 403;
				return std::string();
			}
	}
	return (path);
}

std::string	httpServer::getContent(std::string path, clientSocket& client, ServerConfig& data) {
	int 	bytes = 0;
	int 	totalBytes = 0;
	int ret;
	if (!client.p.chyata.empty())
		return client.p.chyata;
	if (client.p.ex.empty())
		setExtension(path, client.p);
	if (!client.p.to_write && !client.p.fdout) {
		client.p.fdout = open(path.c_str(), O_RDONLY);
		struct stat fileStat;
		if ((ret = stat(path.c_str(), &fileStat)) == 0)
			client.p.to_write = fileStat.st_size;	
		if (client.p.fdout < 0 || ret != 0) {
			client.p.status = 404;
			client.p.fdout = 0;
			return getErrorPageContent(data, client);
		}
	}
	char	buffer[CHUNK_SIZE] = {0};

	if ((bytes = read(client.p.fdout, buffer, CHUNK_SIZE)) < 0) {
		client.p.status = 500;
		getContent(getErrorPageContent(data, client), client, data);
	}
	if (bytes >= 0)
		totalBytes += bytes;
	return std::string(buffer, totalBytes);
}

std::string	httpServer::getErrorPageContent(ServerConfig data, clientSocket& client) {
	std::string	path;
	client.p.ex = std::string(".html");
	if (data.GetErrorPages().find(client.p.status) != data.GetErrorPages().end())
	{
			path += data.GetErrorPages()[client.p.status];   
			if (!path.empty()) {
				std::ifstream	file(path.c_str());
				if (!file.is_open())
					return generateDynamicPage(client.p.status, client.p);
				return getContent(path, client, data);
			}
	}
	return generateDynamicPage(client.p.status, client.p);
}

std::string	httpServer::parseCgiOutput(std::string php_cgi, clientSocket& client, ServerConfig& data) {
	std::stringstream	headers;
	std::stringstream	body;
	std::string			key;

	size_t i = php_cgi.find("\r\n\r\n");
	if (i != std::string::npos) {
		headers << php_cgi.substr(0, i);
		body << php_cgi.substr(i + 4, std::string::npos);
	}
	else {
		client.p.status = 500;
		return getErrorPageContent(data, client);
	}
	while (headers.rdbuf()->in_avail() && !headers.fail()) {
		std::string line;
		if (!getline(headers, line, '\r'))
			throw std::runtime_error("getline failed.");
		if (line[0] == '\n')
			line.erase(0,1);
		i = line.find(": ");
		if (i != std::string::npos) {
			key = line.substr(0, i);
			if (client.p.headers.find(key) == client.p.headers.end())
				client.p.headers[key] = line.substr(i + 2, std::string::npos);
			else
				client.p.headers[key] += line.substr(i + 2, std::string::npos);
		}
		else if (line.find("HTTP") != std::string::npos) {
			i = line.find(" ");
			if (i == std::string::npos)
				throw std::runtime_error("error in php cgi headers.");
			client.p.headers["httpversion"] = line.substr(0, i);
			client.p.headers["status"] = line.substr(i + 1, std::string::npos);
		}

	}
	client.p.to_write = body.str().size();
	return body.str();
}