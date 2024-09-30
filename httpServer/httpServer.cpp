#include "../Headers/WebServerIncludes.hpp"

httpServer::httpServer() {
	setMimeType();
	setStatusPhrase();
}

void	httpServer::checkMatrix(size_t size) {
	for (size_t i = 0; i < size; ++i) {
		std::vector<ServerConfig> servers = data[i];
		int count = 0;
		for (size_t j = 0; j < servers.size(); ++j) {
			if (servers[j].GetDefaultServer())
				count++;				
		}
		if (count > 1)
			throw std::runtime_error("Error: multiple default servers");
	}
}

bool pairExists(std::vector<std::pair<std::string, int> > vec, std::pair<std::string, int>& target) {
    for (std::vector<std::pair<std::string, int> >::iterator it = vec.begin(); it != vec.end(); ++it) {
        if (it->first == target.first && it->second == target.second)
            return true;
    }
    return false;
}

httpServer::httpServer(std::vector<ServerConfig> _servers) {
	setMimeType();
	setStatusPhrase();
	std::vector<std::pair<std::string, int> > serverHosts;
	std::vector<std::pair<std::string, int> >::iterator it;
	std::map<int, std::vector<ServerConfig> >::iterator i;
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::pair<std::string, int> pair = std::make_pair(_servers[i].GetHost(), _servers[i].GetPort());
		if (pairExists(serverHosts, pair))
			continue;
		serverHosts.push_back(std::make_pair(_servers[i].GetHost(), _servers[i].GetPort()));
	}
	setServerMatrix(_servers, serverHosts);
	checkMatrix(serverHosts.size());
	for (std::vector<std::pair<std::string, int> >::iterator it = serverHosts.begin(); it != serverHosts.end(); ++it)
		startingConnection(it->first.c_str(), it->second);
	startingServer();
}

// httpServer::httpServer(httpServer const & obj) {
// 	*this = obj;
// }

httpServer::~httpServer() {}

/*----------------------------------------------- chlada ------------------------------------------------------*/

void	httpServer::parseRequest(char* buffer, clientSocket &client, std::vector<ServerConfig> &data) {
	pars&	p = client.p;
	size_t	pos;
	if (p.headers.find("method") != p.headers.end()) {
		std::string	body(buffer,p.valread);
		redir_to_method(client, getMatchingServer(client.p, data), body);
	}
	else {
		std::string line, body;
		std::string sbuf(buffer, p.valread);
		std::istringstream ss(sbuf);
		size_t i;
		try {
			if (ss.eof() || ss.tellg() != std::stringstream::pos_type(0))
				throw std::runtime_error("Failed to read the request.");
			if (!getline(ss, p.headers["method"], ' ') \
			|| !getline(ss, p.headers["url"], ' ') \
			|| !getline(ss, p.headers["httpversion"], '\r'))
				throw std::runtime_error(gai_strerror(errno));			
		
			ss.str().erase(0);
			if ((pos = p.headers["url"].find("?")) != std::string::npos) {
				p.headers["query"] = p.headers["url"].substr(pos + 1, p.headers["url"].length());
				p.headers["url"] = p.headers["url"].substr(0, pos);
			}
			fflush(stdout);
			while (ss.rdbuf()->in_avail() && !ss.fail()) {
				if (!getline(ss, line, '\r'))
					throw std::runtime_error(gai_strerror(errno));
				if (line[0] == '\n')
					line.erase(0,1);
				i = line.find(": ");
				if (i == std::string::npos) {
					if (ss.peek() == '\n') {
						body = ss.str().substr(ss.tellg());
						body.erase(0, 1);
						break;
					}
				}
				else
					p.headers[line.substr(0, i)] = line.substr(i + 2, std::string::npos);
			}
		} catch (...) {
			client.p.status = 400;
			client.p.end_flag = 1;
			generateResponse(getErrorPageContent(getMatchingServer(client.p, data), client), client);
			return ;
		}
		try {
			is_valid(client);
			redir_to_method(client, getMatchingServer(client.p, data), body);
		} catch (int status) {
			client.p.status = status;
			client.p.end_flag = 1;
			generateResponse(getErrorPageContent(getMatchingServer(client.p, data), client), client);
		}
	}
}

/*----------------------------------------------- methods ------------------------------------------------------*/

void	httpServer::GET(clientSocket& client, ServerConfig& data) {
	if (!client.p.changeEvent) {
		client.p.changeEvent = 1; return ;
	}
	std::string path;
	Location matchedLocation = getMatchingLocation(client.p, data);
	if (checkStatus(client, data))
		return ;
	std::string	root = getRoot(data, matchedLocation);
	if (checkStatus(client, data))
		return ;
	path += getPath(client, data, matchedLocation);;
	erase_consequtive_slash(path);
	if (path.empty() || checkStatus(client, data)) 
		return ;
	setExtension(path, client.p);
	int cgi_valid = check_cgi_2(client, matchedLocation, path);
	if (!cgi_valid && checkStatus(client, data))
		return ;
	else if (cgi_valid == 1) {
		try {
			if (!client.cgi.GetExecutedCgi()) {
				client.cgi.SetScriptPath(path);
				client.cgi.SetExecutablePath(matchedLocation.GetCGI()[client.p.ex.substr(1, std::string::npos)]);
				client.cgi.SetOutfile(generateRandomFilename());
			}

			client.cgi.ExecuteCGI(client.p);

			if (client.cgi.GetCgiStatus())
				return ;
			
			if (client.p.ex == ".php")
				generateResponse(parseCgiOutput(getContent(client.cgi.GetOutfile(), client, data), client, data), client);
			else
				generateResponse(getContent(client.cgi.GetOutfile(), client, data), client);	
			std::remove(client.cgi.GetOutfile().c_str());
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			client.p.status = 500;
			checkStatus(client, data);
		}
	}
    else if (!matchedLocation.GetPath().empty() && matchedLocation.GetDirectoryListing() && CheckDirectory(path))
		generateResponse(generateDirectoryListing(getDirContent(path), client.p, client.p.headers["url"]), client);
    else if (!client.p.file_name.empty())
		generateResponse(std::string(), client);
	else
		generateResponse(getContent(path, client, data), client);	
}

int	httpServer::check_cgi_2(clientSocket& client, Location& matchedLocation, std::string path) {
	setExtension(path, client.p); 
	if (client.p.status >= 400)
		return 0;
	if (client.p.ex != ".php" && client.p.ex != ".py")
		return -1;
	std::map<std::string, std::string> cgi = matchedLocation.GetCGI();
	if (!cgi.size() || cgi.find(client.p.ex.substr(1, std::string::npos)) == cgi.end()) {
		std::cout << client.p.ex.substr(1, std::string::npos);
		client.p.status = 403; return 0;
	}
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) != 0){
		client.p.status = 404; return 0;
	}
	if ((fileStat.st_mode & S_IFREG) && (fileStat.st_mode & S_IXUSR)) {
		client.p.status = 403; return 0;
	}
	struct stat _fileStat;
	if (stat(cgi[client.p.ex.substr(1, std::string::npos)].c_str(), &_fileStat) != 0) {
		client.p.status = 500; return 0;
	}
	if ((fileStat.st_mode & S_IFREG) && (fileStat.st_mode & S_IXUSR)) {
		client.p.status = 403; return 0;
	}
	return 1;
}

void	httpServer::DELETE(clientSocket& client, ServerConfig& data) {
	Location matchedLocation = getMatchingLocation(client.p, data);
	if (checkStatus(client, data))
		return ;
	std::string url = client.p.headers["url"];
	std::map<std::string,std::string> headers = client.p.headers;
	std::string path = getPath(client, data, matchedLocation);
	if (checkStatus(client, data))
		return ;	
	std::vector<std::string> dirContent = getDirContent(path);
	if (dirContent.size() != 0)
		delete_dir_content(path, dirContent, client);
	else if (access(path.c_str(), F_OK | W_OK) == 0) {
		if (std::remove(path.c_str()) != 0)
			client.p.status = 403;
	}
	else
		client.p.status = 403;
	if (checkStatus(client, data))
		return ;
	client.p.status = 204;
	generateResponse(generateDynamicPage(client.p.status, client.p), client);
}
