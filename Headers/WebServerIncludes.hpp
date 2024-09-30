/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerIncludes.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 21:42:11 by gkarib            #+#    #+#             */
/*   Updated: 2023/07/08 16:50:29 by gkarib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVERINCLUDES_HPP
#define WEBSERVERINCLUDES_HPP

class   Location;   

class   ServerConfigParser;

class	HTTPserverParsing;

class	ConfigFile;

#include "Libraries.hpp"

#include "Cgi.hpp"

#include "ConfigFileParsing/Location.hpp"

#include "ConfigFileParsing/ServerConfig.hpp"

#include "ConfigFileParsing/ConfigFile.hpp"

extern std::vector<pid_t> pids;

#define backlog 512 // for a high incoming connection rate
#define test_response "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"
#define	CHUNK_SIZE 30000

// unused macros
#define M_H 8000
#define M_B 31884568129
#define UPLOADED_FILE "fototita.mp4"
#define	FILE_SIZE 31884568129

class pars
{
	public :
		// vars i be using , if u already use one of them and u think it might cause a problem notify me
		std::string							chyata; // stores the bytes left from a send operation
		int									status; // stores the status code
		size_t								written; // check how many bytes i have sent
		size_t								to_write; // stores the size of the file to be sent
		int									changeEvent; // a flag to change the client's event
		std::string							file_name; // stores the location to be redirected to if needed
		std::map<std::string,std::string>	headers; // map dlheaders ^^
		std::string							ex; // stores the extension of the file
		int									fdout; // stores the fd of the file to be sent
		int									end_flag; // a flag to end the connection
		// ------------------------------ unused vars ------------------------------
		// int									type;
		// int									err;
		// int									bounce_client;
		// ssize_t 							max; //init some macro;
		// std::string							host;
		// ------------------------------ vars u use ------------------------------
		std::string							ful_path;
		int									cgi_serv;//init 0;
		int									f_open;// init 0;
		int									up_fd;// no init needed;
		std::string							cgi_file;
		size_t								hex_l; //init 0;
		size_t								to_be_skip; //init 1 !!!;
		std::string							hexa; //the hexa value as a string
		size_t								chunk_n;
		ssize_t 							t_valread; //init 0;
		ssize_t 							valread; //init 0;
		ssize_t 							cont_l; //init 0;
		size_t 								s; //init 0;
		pars();
		pars(pars const & obj);
		pars& operator=(pars const & obj);
		void	upd_valread(ServerConfig &data);
};

class clientSocket {
	public :
		pars			p;
		int				socket_fd;
		Cgi				cgi;
		clientSocket(int fd);
		clientSocket(const clientSocket& obj);
		clientSocket& operator=(clientSocket const & obj);
};

class serverSocket {
	public:
		int							socket_fd;
		struct sockaddr_in			address;
		socklen_t					addrLength;
		std::vector<clientSocket>	clients;
};

class httpServer {
	private:
		/*--------------------------------------------------Data  memebers-----------------------------------------------*/
		std::vector<serverSocket>							server_socket;
		std::map<int,std::vector<ServerConfig> >			data;
		std::vector<std::string>							cgi_bodies;
		/*------------------------------------------------------Network---------------------------------------------------*/
		void						startingConnection(const char* interface, int port);
		void						startingServer();
		struct pollfd				*add_client_fd(struct pollfd *fd_poll, size_t *nfds, int fd);
		struct pollfd				*rm_client_fd(struct pollfd *fd_poll, size_t *nfds, int fd);
		int							check_conf_p(clientSocket &client, pars &p, ServerConfig &data);
		void						handle_connection(clientSocket &client, std::vector<ServerConfig> &data, serverSocket &server, struct pollfd **fd_poll, size_t *nfds, int i);
		void						b_client(clientSocket &client, serverSocket &server, size_t *nfds, struct pollfd **fd_poll);
		/*------------------------------------------------------Send and receive---------------------------------------------------*/
		void						parseRequest(char* buffer, clientSocket &client, std::vector<ServerConfig>& data);
		void						generateResponse(std::string body, clientSocket& client);
		/*---------------------------------------------------------getters---------------------------------------------------------*/
		std::string					getRoot(ServerConfig data, Location& matchedLocation);
		std::string					getPath(clientSocket& client, ServerConfig& data, Location& matchedLocation);
		Location					getMatchingLocation(pars &p, ServerConfig& data);
		std::string					getContent(std::string path,clientSocket& client, ServerConfig& data);
		std::vector<std::string>	getDirContent(std::string& path);
		std::string					getErrorPageContent(ServerConfig data, clientSocket& client);
		ServerConfig				getMatchingServer(pars& p, std::vector<ServerConfig>& data);
		std::string					generateDirectoryListing(std::vector<std::string> dirContent, pars& p, std::string root);
		std::string					generateDynamicPage(int status, pars& p);
		/*---------------------------------------------------------setters---------------------------------------------------------*/
		void						setStatusPhrase(void);
		void						setMimeType(void);
		void						setExtension(std::string filename, pars& p);
		void						setServerMatrix(std::vector<ServerConfig> _servers, std::vector<std::pair<std::string, int> > serverHosts);
		/*---------------------------------------------------------helpers---------------------------------------------------------*/
		void						redir_to_method(clientSocket& client, ServerConfig data, std::string& body);
		void						is_valid(clientSocket& client);
		int							check_cgi(pars &p, Location &m_loc, std::string &root);
		void						delete_dir_content(std::string	path, std::vector<std::string>& dirContent, clientSocket& client);
		bool						checkStatus(clientSocket& client, ServerConfig& data);
		int							check_cgi_2(clientSocket& client, Location& matchedLocation, std::string path);
		std::string					parseCgiOutput(std::string php_cgi, clientSocket& client, ServerConfig& data);
		void						checkMatrix(size_t size);
		void						handle_cgi(clientSocket &client, ServerConfig &data);
		/*----------------------------------------------------------HTTP methods------------------------------------------------------*/
		void						GET(clientSocket& client, ServerConfig& data); // i will just ignore the host header for the moment
		void						POST(std::string body, clientSocket &client, ServerConfig &data);
		void						DELETE(clientSocket& client, ServerConfig& data);
	public:
		httpServer();
		httpServer(std::vector<ServerConfig> _servers);
		httpServer(httpServer const & obj);
		~httpServer();
		std::map<int,std::string>							status_phrase;
		std::map<std::string,std::string>					mime_type;
};

void		erase_consequtive_slash(std::string& url);
void		sigHandler(int sigNum);
void		sort_location(std::vector<Location>& location);
bool		isSocketOpen(int socket_fd);
int			ft_remove(const char* path, int mode);
std::string	generateRandomFilename();
void		killChildren();
void		add_pid(int pid);
void		ft_signal();
#endif