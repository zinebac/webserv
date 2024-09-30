#include "../Headers/WebServerIncludes.hpp"


std::vector<pid_t> pids;

// check if rand and srand are allowed functions
std::string	generateRandomFilename() {
    std::string filename;
    do {
        std::stringstream randomNumber;
		randomNumber << rand() % 10000;
        filename = "/tmp/" + randomNumber.str();
    } while (std::ifstream(filename));

    return filename;
}

void	killChildren() {
    for (size_t i = 0; i < pids.size(); ++i)
        kill(pids[i], SIGKILL);
}

void	ft_signal() {
	std::signal(SIGPIPE, SIG_IGN);
	std::signal(SIGINT, sigHandler);
	std::signal(SIGTERM, sigHandler);
	std::signal(SIGQUIT, sigHandler);
	std::signal(SIGKILL, sigHandler);
	std::signal(SIGABRT, sigHandler);
	std::signal(SIGSTOP, sigHandler);
	std::signal(SIGTSTP, sigHandler);
	// std::signal(SIGSEGV, sigHandler);
}

void	sigHandler(int sigNum) {
	if (sigNum == SIGINT || sigNum == SIGTERM || sigNum == SIGQUIT
		|| sigNum == SIGABRT || sigNum == SIGKILL || sigNum == SIGSTOP || sigNum == SIGTSTP) // all i could think of
	{
		killChildren();
		exit(sigNum);
	} // just for debugging neither of these signals should be caught anyway
	else if (sigNum == SIGSEGV) {
		std::cerr << "segmentation fault signal caught" << std::endl;
	} else if (sigNum == SIGPIPE) {
		std::cerr << "broken pipe signal caught" << std::endl;
	}
}

void	add_pid(int pid) {
	if (std::find(pids.begin(), pids.end(), pid) == pids.end())
		pids.push_back(pid);
}

void	sort_location(std::vector<Location>& location) {
	for (size_t i = 0; i < location.size(); ++i) {
		for (size_t  j = i + 1; j < location.size(); ++j) {
			if (location[i].GetPath() < location[j].GetPath()) {
				Location tmp = location[i];
				location[i] = location[j];
				location[j] = tmp;
			}
		}
	}
}

void	httpServer::is_valid(clientSocket& client) {
	std::map<std::string,std::string>& headers = client.p.headers;
	if (headers["httpversion"] != "HTTP/1.1")
		throw(505);
	if (headers["method"] != "GET" && headers["method"] != "POST" && headers["method"] != "DELETE")
		throw(501);
	if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] != "chunked" )
		throw(501);
	if (headers["method"] == "POST" && headers.find("Transfer-Encoding") == headers.end() && headers.find("Content-Length") == headers.end())
		throw(411);
	if (headers["url"].length() > 2048)
		throw(414);
	if (headers["url"].find("{}[]\n\r\t\v\f\\\"\'<>~") != std::string::npos)
		throw(400);
}

void	httpServer::redir_to_method(clientSocket& client, ServerConfig data, std::string& body) {
	pars& p = client.p;
	if (p.headers["method"] == "GET")
		GET(client, data);
	else if (p.headers["method"] == "POST") {
		fflush(stdout);
		POST(body, client, data);
	}
	else if (p.headers["method"] == "DELETE")
		DELETE(client, data);
}

void	erase_consequtive_slash(std::string& url)
{
	size_t i = 0;
    while (i < url.length())
    {
        if (url[i] == '/' && url[i + 1] == '/')
        {
            url.erase(i, 1);
            i = 0;
            continue;
        }
        i++;
    }
}

bool	httpServer::checkStatus(clientSocket& client, ServerConfig& data) {
	if (client.p.status >= 400) {
		generateResponse(getErrorPageContent(data, client), client);
		return true;
	}
	return false;
}

int ft_remove(const char* path, int mode) {
	if (access(path, mode) == 0) {
		if (std::remove(path) == 0)
			return 200;
	}
	return 403;
}

void	httpServer::delete_dir_content(std::string	path, std::vector<std::string>& dirContent, clientSocket& client) {
	std::string newPath;
	for (size_t i = 0; i < dirContent.size(); ++i) {
		if (dirContent[i] == "." || dirContent[i] == "..")
			continue ;
		newPath = path + "/" + dirContent[i];
		if (CheckDirectory(newPath)) {
			std::vector<std::string>	newDirContent = getDirContent(newPath);
			delete_dir_content(newPath, newDirContent, client);
			if (client.p.status == 403)
				return ;
		}
		else if ((client.p.status = ft_remove(newPath.c_str(), F_OK | W_OK)) != 200)
			return ;	
	}
}
