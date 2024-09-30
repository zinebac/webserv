/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ghita <ghita@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 11:54:03 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/07 19:47:00 by ghita            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/WebServerIncludes.hpp"

ConfigFile::ConfigFile()
{
	_Servers.clear();
	_Servers = std::vector<ServerConfig>();
}

ConfigFile::~ConfigFile()
{
	_Servers.clear();
}

ConfigFile::ConfigFile (const ConfigFile& copy)
{
	this->_Servers = copy._Servers;
}


ConfigFile& ConfigFile::operator=(const ConfigFile& copy)
{
	this->_Servers = copy._Servers;
	return (*this);
}

// --------------------------------- GETTERS ---------------------------------

const std::vector<ServerConfig>&  ConfigFile::GetServersConfig( void )  const
{
	return (this->_Servers);
}

// --------------------------------- SETTERS ---------------------------------

void    ConfigFile::SetServers(std::vector<ServerConfig> Server)
{
	this->_Servers = Server;
}

// --------------------------------- OTHER FUNCTIONS ---------------------------------

bool    ConfigFile::isServerAdded(const ServerConfig& server)
{
	for (size_t i = 0; i < _Servers.size(); ++i)
	{
		// Compare the relevant fields to determine if the server is already added
		if (server.GetHost() == _Servers[i].GetHost() && server.GetPort() == _Servers[i].GetPort() && server.GetServerNames() == _Servers[i].GetServerNames())
			throw std::runtime_error(std::string("Error: Duplicated Server in Config File."));
	}
	return false;
}

void	ConfigFile::ResolvableHost_Port(ServerConfig currentServer)
{
	struct addrinfo hints;
	struct addrinfo *result;
	int status;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(currentServer.GetHost().c_str(), NULL, &hints, &result);
	if (status != 0)
		throw std::runtime_error(std::string("Error: getaddrinfo() failed. ") + gai_strerror(status));
	freeaddrinfo(result);
}


void    ConfigFile::read_ConfigFile()
{
	std::string line;
	std::stack<char> brace;
	bool serverContextFound = false;
	ServerConfig currentServer;

	int braceBalance = 0;

	while (getline(_Config_file, line))
	{
		line = isComment(line);
		line = trim_white_spaces(line);

		if (!line.empty())
		{
			if (!CheckBrace(line, braceBalance))
				return ;
			if (DefineContext(line) == SERVER)
			{
				serverContextFound = true;
				continue ;
			}

			if (DefineContext(line) != SERVER && !serverContextFound)
				throw std::runtime_error(std::string("Error: Expected 'server' context. "));	
			
			
			currentServer.fill_ServerInfos(line, _Config_file, braceBalance);
		
			if (line == "}")
			{
				if (!isServerAdded(currentServer))
				{
					_Servers.push_back(currentServer);
					if (currentServer.isServerConfigFilled())
						throw std::runtime_error(std::string("Error: An important element isn't found in a server context. "));
					ResolvableHost_Port(currentServer);
					currentServer.clearServerConfig(); // Clear the currentServer object for each new server definition
				}
			}
		}
	}

	if (braceBalance > 0)
        throw std::runtime_error(std::string("Error: Unmatched opening brace '{' at the end of the file. "));

	_Config_file.close();
}


void    ConfigFile::Check_Conf_file(const char *file_name)
{
	if (!check_extension(file_name, ".conf"))
		return;
	open_file(file_name);
	read_ConfigFile();
}