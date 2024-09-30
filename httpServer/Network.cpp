#include "../Headers/WebServerIncludes.hpp"

void	httpServer::startingConnection(const char* interface, int port) {
	int	opts = 1;
	serverSocket	tmp;
	memset((char *)&tmp.address, 0, sizeof(tmp.address));
	tmp.address.sin_family = AF_INET;
	tmp.address.sin_port = htons(port);
	if (interface == std::string("localhost"))
		tmp.address.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		tmp.address.sin_addr.s_addr = inet_addr(interface);
	tmp.address.sin_addr.s_addr = inet_addr(interface);
	memset(tmp.address.sin_zero, '\0', sizeof(tmp.address.sin_zero));
	tmp.addrLength = sizeof(tmp.address);
	tmp.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tmp.socket_fd  < 0)
		throw std::runtime_error(std::string("socket failed: ") + gai_strerror(errno));
	setsockopt(tmp.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opts, sizeof(int));
	if (bind(tmp.socket_fd, (struct sockaddr*)&tmp.address, tmp.addrLength) < 0)
		throw std::runtime_error(std::string("bind failed: ") + gai_strerror(errno));
	if (fcntl(tmp.socket_fd, F_SETFL, O_NONBLOCK))
		throw std::runtime_error("error starting one of the servers");
	if (listen(tmp.socket_fd, backlog) < 0)
		throw std::runtime_error(std::string("listen failed: ") + gai_strerror(errno));
	server_socket.push_back(tmp);
}

struct pollfd *httpServer::rm_client_fd(struct pollfd *fd_poll, size_t *nfds, int fd)
{
	size_t tmp = (*nfds) - 1;
	struct pollfd *r = new struct pollfd[tmp];
	if (r == NULL)
		throw std::runtime_error("Memmory allocation failed!!\n throw this calculator ffs!\n");
	size_t i = 0;
	size_t c = 0;
	while (i < *nfds && c < tmp)
	{
		if (fd_poll[i].fd != fd)
		{
			r[c] = fd_poll[i];
			c++;
		}
		i++;
	}
	delete[] fd_poll;
	(*nfds)--;
	return (r);
}

void	_deb(size_t s, struct pollfd *f)
{
	for (size_t k = 0; k < s; k++)
	{
		std::cout << " |";
		if (isSocketOpen(f[k].fd))
			std::cout << "====> Open " << f[k].fd << std::endl; 
		else
			std::cout << "====> Closed " << f[k].fd << std::endl; 
	}
	std::cout << std::endl;
	(void)s;
	(void)f;
}

struct pollfd *httpServer::add_client_fd(struct pollfd *fd_poll, size_t *nfds, int fd) //we need to add clients to the fd set in order to get notified about the events
{
	//std::cout << "this is the clinet to be added " << fd << std::endl;
	struct pollfd *r = new struct pollfd[(*nfds) + 1];
	if (r == NULL)
		throw std::runtime_error("Memmory allocation failed!!\n throw this calculator ffs!\n");
	for (size_t i = 0; i < (*nfds); i++)
		r[i] = fd_poll[i];
	r[*nfds].fd = fd;
	r[*nfds].events = POLLIN;
	r[*nfds].revents = 0;
	delete[] fd_poll;
	(*nfds)++;
	//std::cout << "inside the add_function ==>" << std::endl;
	// _deb(*nfds, r);
	return (r);
}

void	httpServer::b_client(clientSocket &client, serverSocket &server, size_t *nfds, struct pollfd **fd_poll)
{
		*fd_poll = rm_client_fd(*fd_poll, nfds, client.socket_fd);
		close(client.socket_fd);
		std::vector<clientSocket>::iterator c_i = server.clients.begin();	

		while (c_i != server.clients.end() && c_i->socket_fd != client.socket_fd)
			c_i++;
		server.clients.erase(c_i);
}

void	httpServer::handle_connection(clientSocket &client, std::vector<ServerConfig> &data, serverSocket &server, struct pollfd **fd_poll, size_t *nfds, int i)
{
	fflush(stdout);
	if ((client.p.end_flag))
	{
		char	r_buff[CHUNK_SIZE] = {0};
		fflush(stdout);
		if (!client.p.changeEvent)
			client.p.valread = read(client.socket_fd, r_buff, CHUNK_SIZE);
		try
		{
			if (client.p.valread < 0)
				throw std::runtime_error("eof reached");

			parseRequest(r_buff, client, data);
		}
		catch (...)
		{
			client.p.end_flag = 0;
		}
	}
	if (!client.p.end_flag)
			b_client(client, server, nfds, fd_poll);
	else if (client.p.changeEvent)
		(*fd_poll)[i].events = POLLOUT;
}

void	httpServer::startingServer()
{
	size_t c_num = server_socket.size();
	size_t s_num = c_num;
	short	flag = 0;
	struct pollfd	*fd_poll = new struct pollfd[c_num];
	if (fd_poll == NULL)
		throw std::runtime_error("Memmory allocation failed!!\n throw this calculator ffs!\n");
	ssize_t				nfds;
	int					tmp_fd;

	for (size_t i = 0; i < c_num; ++i)
	{
		fd_poll[i].fd = server_socket[i].socket_fd;
		fd_poll[i].events = POLLIN;
	}
	while (1)
	{
		if ((nfds = poll(fd_poll, c_num, -1)) < 0)
			throw std::runtime_error(strerror(errno));
		for (size_t i = 0; i < c_num; ++i)
		{
			if (fd_poll[i].revents & POLLIN || fd_poll[i].revents & POLLHUP || fd_poll[i].revents & POLLOUT || fd_poll[i].revents & POLLERR)
			{
				flag = 0;
				fflush(stdout);
				for(size_t	j = 0; j < s_num; j++)
				{
					if (flag)
					{
						flag = 0;
						break;
					}
					if (server_socket[j].socket_fd == fd_poll[i].fd)
					{
						if ((tmp_fd = accept(server_socket[j].socket_fd, (struct sockaddr*)&(server_socket[j].address), &(server_socket[j].addrLength))) <  0)
						{
							if (fd_poll[i].revents & POLLIN)
							throw std::runtime_error("Error occured when accepting connection");
						}
						if (fcntl(tmp_fd, F_SETFL, O_NONBLOCK) < 0)
							throw std::runtime_error("yalaatiiif");

						std::vector<clientSocket>::iterator client = server_socket[j].clients.begin();
						while (client != server_socket[j].clients.end() && client->socket_fd != tmp_fd)
							client++;
						if (client  == server_socket[j].clients.end())
						{
							clientSocket tmp(tmp_fd);
							server_socket[j].clients.push_back(tmp);
							client = server_socket[j].clients.end() - 1;
						}
						else
							throw std::runtime_error("achhad lemlawi!!");
	
						fd_poll = add_client_fd(fd_poll, &c_num, tmp_fd);
                        break;
					}
					else
					{
						for(size_t c = 0; c < server_socket[j].clients.size(); c++)
						{
							if (fd_poll[i].fd == server_socket[j].clients[c].socket_fd)
							{
								if ((fd_poll[i].revents & POLLHUP))
								{
									// std::cout << "end_flag ==> " << server_socket[j].clients[c].p.end_flag << std::endl;
									// std::cout << "fdout ==> " << server_socket[j].clients[c].p.fdout << std::endl;
									if (server_socket[j].clients[c].p.headers["method"] == "GET" && server_socket[j].clients[c].p.end_flag && server_socket[j].clients[c].p.fdout > 0)
										close(server_socket[j].clients[c].p.fdout);
									if (server_socket[j].clients[c].p.headers["method"] == "POST" && !server_socket[j].clients[c].p.changeEvent && server_socket[j].clients[c].p.up_fd > 0)
										close(server_socket[j].clients[c].p.up_fd);
									server_socket[j].clients[c].p.end_flag = 0;
									//important check if input or output of th ecg is open and also kill any infinite loop script;
									// exit(8);
								}
								handle_connection(server_socket[j].clients[c], data[j], server_socket[j], &fd_poll, &c_num, i);
								flag = 1;
								break;
							}
						}
					}
				}
			}
		}
	}
}