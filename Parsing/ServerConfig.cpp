/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/01 01:09:24 by ghita             #+#    #+#             */
/*   Updated: 2023/08/12 06:57:11 by gkarib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/WebServerIncludes.hpp"


ServerConfig::ServerConfig()
{
	SetHost("127.0.0.1");
	SetPort(8080);
	SetRoot("");
	SetServerNames("");
	SetDefaultServer(false);
	SetErrorPages(std::map<int, std::string>());
	SetLimitClientBodySize(0);
	SetLocation(std::vector<Location>());
}

ServerConfig::~ServerConfig()
{
}

ServerConfig::ServerConfig(const ServerConfig& other)
{
	this->_Host = other._Host;
	this->_Root = other._Root;
	this->_Port = other._Port;
	this->_ServerNames = other._ServerNames;
	this->_DefaultServer = other._DefaultServer;
	this->_ErrorPages = other._ErrorPages;
	this->_LimitClientBodySize = other._LimitClientBodySize;
	this->_Locations = other._Locations;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
	if (this != &other)
	{
		this->_Host = other._Host;
		this->_Root = other._Root;
		this->_Port = other._Port;
		this->_ServerNames = other._ServerNames;
		this->_DefaultServer = other._DefaultServer;
		this->_ErrorPages = other._ErrorPages;
		this->_LimitClientBodySize = other._LimitClientBodySize;
		this->_Locations = other._Locations;
	}
	return (*this);
}

// --------------------------------- GETTERS ---------------------------------

std::string	ServerConfig::GetHost( void ) const
{
	return (this->_Host);
}

std::string	ServerConfig::GetRoot( void ) const
{
	return(this->_Root);
}

int	ServerConfig::GetPort( void ) const
{
	return (this->_Port);
}

std::string	ServerConfig::GetServerNames( void ) const
{
	return (this->_ServerNames);
}

bool	ServerConfig::GetDefaultServer( void ) const
{
	return (this->_DefaultServer);
}

std::map<int, std::string>	ServerConfig::GetErrorPages( void ) const
{
	return (this->_ErrorPages);
}

int	ServerConfig::GetLimitClientBodySize( void ) const
{
	return (this->_LimitClientBodySize);
}

std::vector<Location>	ServerConfig::GetLocation( void ) const
{
	return (this->_Locations);
}

// --------------------------------- SETTERS ---------------------------------

void	ServerConfig::SetHost(const std::string& host)
{
	this->_Host = host;
}

void	ServerConfig::SetRoot(const std::string& root)
{
	this->_Root = root;
}

void	ServerConfig::SetPort(int port) {
	this->_Port = port;
}

void	ServerConfig::SetServerNames(const std::string& serverName)
{
	this->_ServerNames = serverName;
}

void	ServerConfig::SetDefaultServer(bool defaultServer)
{
	this->_DefaultServer = defaultServer;
}

void	ServerConfig::SetErrorPages(const std::map<int, std::string>& errorPages)
{
	this->_ErrorPages = errorPages;
}

void	ServerConfig::SetLimitClientBodySize(int limitClientBodySize)
{
	this->_LimitClientBodySize = limitClientBodySize;
}

void	ServerConfig::SetLocation(const std::vector<Location>& location)
{
	this->_Locations = location;
}

// --------------------------------- OTHER FUNCTIONS ---------------------------------

bool	ServerConfig::isServerConfigFilled()
{		
		return (GetHost().empty() ||
				GetRoot().empty());
}

void	ServerConfig::clearServerConfig()
{
	// Reset the state of the server object
	SetHost("");
	SetRoot("");
	SetPort(0);
	SetServerNames("");
	SetDefaultServer(false);
	SetErrorPages(std::map<int, std::string>());
	SetLimitClientBodySize(0);
	SetLocation(std::vector<Location>());
}

bool	ServerConfig::ValidKey(std::string key)
{
	return (key != "host"
		&& key != "port"
		&& key != "root"
		&& key != "server_names"
		&& key != "default_server"
		&& key != "error_pages"
		&& key != "limit_client_body_size"
		&& key != "location"
		&& key != "}");
}

std::map<int, std::string>	ServerConfig::fill_ErrorPages(std::ifstream& config_file, int &braceBalance)
{
	std::map<int, std::string> Map;
	std::string errorline;
	
	while (getline(config_file, errorline))
	{
		errorline = trim_white_spaces(isComment(errorline));
		if (!errorline.empty())
		{
			if (errorline.find("}") != std::string::npos)
			{
				CheckBrace(errorline, braceBalance);
				break;
			}
			
			std::istringstream iss(errorline);
			int key;
			std::string value;
			
			if (!(iss >> key))
				throw std::runtime_error("Error: Invalid number for Error Pages format: in line " + errorline + " " +gai_strerror(errno));
			iss >> value;
			if (value[0] != '/' && value[0] != '.')
				value.insert(0, "/");

			Map[key] = value;
			if (iss >> value)
				throw std::runtime_error(std::string("Error: Too many values found in Error_Pages. ") + gai_strerror(errno));
		}
	}
	return (Map);
}


void	ServerConfig::fill_ServerInfos(const std::string& line, std::ifstream& config_file, int& braceBalance)
{
	(void) config_file;
	std::istringstream iss(line);
	std::string key;
	std::string value;

	if (!(iss >> key))
		throw std::runtime_error("Error: Invalid key in Server config for " + key + gai_strerror(errno));
	if (ValidKey(key) && key != "{")
		throw std::runtime_error("Error: Invalid key in Server config for " + key + gai_strerror(errno));
		
	if (line.find("}") != std::string::npos)
		return ;
	
	if (iss >> value)
	{
		std::string	word;
		if (iss >> word && key != "location")
			throw std::runtime_error("Error: To many values found for " + key);
	} 
	else
	{
		if (key == "{")
			throw std::runtime_error("Error: Unmatched opening brace '{'");
		throw std::runtime_error("Error: No value found for " + key + " in Server config. ");
	}


	if (key == "host")
	{
		if (!value.compare("localhost"))
			this->SetHost("127.0.0.1");
		else if (!value.compare("0.0.0.0"))
			throw std::runtime_error("Error: Invalid host value: " + value);
		else
			this->SetHost(value);
	}
	else if (key == "root")
	{
		if (value[0] != '/' && value[0] != '.')
			value.insert(0, "/");
		this->SetRoot(value);
	}
	else if (key == "port")
	{
		std::istringstream iss(value);
		int port_value = 0;
		if (!(iss >> port_value) || !iss.eof())
			throw std::runtime_error("Error: Invalid integer for port format: " + value + " " + gai_strerror(errno));
		if (port_value < 1024 || port_value > 49151)
			throw std::runtime_error("Error: Port out of range: " + value);
		this->SetPort(port_value);
	}
	else if (key == "server_names")
	{	
		this->SetServerNames(value);
	}
	else if (key == "default_server")
	{
		if (value == "false")
			this->SetDefaultServer(false);
		else if (value == "true")
			this->SetDefaultServer(true);
		else 
			throw std::runtime_error("Error: Invalid default_server value: " + value);
	}
	else if (key == "error_pages")
	{
		this->SetErrorPages(fill_ErrorPages(config_file, braceBalance));
		
		std::map<int, std::string>::const_iterator iter;
		for(iter = this->_ErrorPages.begin(); iter != this->_ErrorPages.end(); iter++)
		{
			if (iter->first < 400 || iter->first > 600)
				throw std::runtime_error("Error: Error_pages status out of range");
		}
	}
	else if (key == "limit_client_body_size")
	{
		// std::cout << "Limit client body size" << std::endl;
		int	limit_value = 0;
		std::istringstream iss(value);
		if (!(iss >> limit_value) || !iss.eof())
			throw std::runtime_error("Error: Invalid integer for limit_client_body_size format: " + value + " " + gai_strerror(errno));
		if (limit_value > 2147483448 || limit_value < 0)
			throw std::runtime_error("Error: limit_client_body_size cannot be negative or greater than 7000000");
		this->SetLimitClientBodySize(limit_value);
	}
	else if (key == "location")
	{
		Location location;
		location.fill_locationInfos(config_file, line, braceBalance);
		if(location.isLocationConfigFilled())
			throw std::runtime_error("Error: Methods and Redirect are missing from location context");
		
		for (size_t i = 0; i < GetLocation().size(); i++)
		{
			if (GetLocation()[i].GetPath() == location.GetPath())
				throw std::runtime_error("Error: Duplicate location path.");
		}
		this->_Locations.push_back(location);
	}
}