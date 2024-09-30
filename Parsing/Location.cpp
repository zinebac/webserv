/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 10:47:52 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/11 21:36:57 by gkarib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/WebServerIncludes.hpp"

Location::Location()
{
	SetPath("");
	SetRoot("");
	SetIndex("");
	SetMethods(std::vector<std::string>());
	SetDirectoryListing(0);
	SetDefaultFile("");
	SetCGI(std::map<std::string, std::string>());
	SetDirectory("");
	SetFileUpload(std::map<std::string, std::string>());
}

Location::~Location()
{
}

Location ::Location(const Location& other)
{
	*this = other;
}

Location&	Location::operator=(const Location& other)
{
	if (this != &other)
	{
		this->_Path = other._Path;
		this->_Root = other._Root;
		this->_Index = other._Index;
		this->_Methods = other._Methods;
		this->_Cgi = other._Cgi ;
		this->_FileUpload = other._FileUpload;
		this->_Redirect = other._Redirect;
		this->_Directory = other._Directory;
		this->_DirectoryListing = other._DirectoryListing;
		this->_DefaultFile = other._DefaultFile;
	}
	return (*this);
}

// --------------------------------- GETTERS ---------------------------------

std::string	Location::GetPath( void ) const
{
	return(this->_Path);
}

std::string	Location::GetRoot( void ) const
{
	return(this->_Root);
}

std::string	Location::GetIndex( void ) const
{
	return(this->_Index);
}

std::vector<std::string>	Location::GetMethods( void ) const
{
	return(this->_Methods);
}

bool	Location::GetDirectoryListing( void ) const
{
	return(this->_DirectoryListing);
}

std::string	Location::GetDefaultFile( void ) const
{
	return(this->_DefaultFile);
}

std::map<std::string, std::string>	Location::GetCGI( void ) const
{
	return(this->_Cgi);
}

std::map<std::string, std::string>	Location::GetFileUpload( void ) const
{
	return (this->_FileUpload);
}

std::string	Location::GetRedirect( void ) const
{
	return(this->_Redirect);
}

std::string	Location::GetDirectory( void ) const
{
	return(this->_Directory);
}

// --------------------------------- SETTERS ---------------------------------

void	Location::SetPath(const std::string& path)
{
	this->_Path = path;
}

void	Location::SetRoot(const std::string& root)
{
	this->_Root = root;
}

void	Location::SetIndex(const std::string& index)
{
	this->_Index = index;
}

void	Location::SetMethods(const std::vector<std::string>& methods)
{
	this->_Methods = methods;
}

void	Location::SetDirectoryListing(bool _DirectoryListing)
{
	this->_DirectoryListing = _DirectoryListing;
}

void	Location::SetDefaultFile(const std::string& defaultFile)
{
	this->_DefaultFile = defaultFile;
}

void	Location::SetCGI(const std::map<std::string, std::string>& Cgi)
{
	this->_Cgi = Cgi;
}

void	Location::SetFileUpload(const std::map<std::string, std::string>& fileUpload)
{
	this->_FileUpload = fileUpload;
}

void Location::SetRedirect(const std::string& redirect)
{
	this->_Redirect = redirect;
}

void	Location::SetDirectory(const std::string& _Directory)
{
	this->_Directory = _Directory;
}

// --------------------------------- OTHER FUNCTIONS ---------------------------------

bool	Location::isLocationConfigFilled( void )
{
	return (GetMethods().empty() && GetRedirect().empty());
}

void	Location::clearLocation( void )
{
	// Reset the state of the server object
	SetMethods(std::vector<std::string>());
	SetDirectoryListing(0);
	SetDefaultFile("");
	SetCGI(std::map<std::string, std::string>());
	SetDirectory("");
	SetFileUpload(std::map<std::string, std::string>());
}

void	Location::GetPath_location(const std::string& line)
{
	std::string path;
	
	path = trim_white_spaces(line.substr(strlen("location") + 1, line.find('{') - strlen("location") - 1));
	if (path.empty())
		throw std::runtime_error(std::string("Error: Location path is empty. ") + gai_strerror(errno));
	if (path[0] != '/' && path[0] != '.')
			path.insert(0, "/");
	this->SetPath(path);
}

std::map<std::string, std::string>	fill_map(std::ifstream &config_file, std::string keyWord, int& braceBalance)
{
	std::map<std::string, std::string> Map;
	std::string mapline;
	
	while (getline(config_file, mapline))
	{
		mapline = trim_white_spaces(isComment(mapline));
		
		if (!mapline.empty())
		{
			if (mapline.find("}") != std::string::npos)
			{
				CheckBrace(mapline, braceBalance);
				break;
			}
			if (mapline.find("{") != std::string::npos)
				CheckBrace(mapline, braceBalance);
			
			std::istringstream iss(mapline);
			std::string key;
			std::string value;
			
			iss >> key;
			iss >> value;
			if (keyWord == "cgi")
			{
				if (value[0] != '/' && value[0] != '.')
					value.insert(0, "/");
			}
			if (keyWord == "file_upload" && key == "directory")
			{
				if (value[0] != '/' && value[0] != '.')
					value.insert(0, "/");
			}
			Map[key] = value;
			if (iss >> value) // check if there is another word after the value if yes throw exception
				throw std::runtime_error(std::string("Error: Too many values found for ") + key + " in location context." + gai_strerror(errno));
		}
	}
	return (Map);
}

bool	Location::ValidKey(std::string key)
{
	return (key != "root"
		&& key != "default_file"
		&& key != "methods"
		&& key != "directory_listing"
		&& key != "cgi"
		&& key != "file_upload"
		&& key != "redirect"
		&& key != "directory"
		&& key != "}");
}

bool	Location::DuplicateMethod( void )
{
	for(size_t i = 0; i < GetMethods().size(); i++)
	{
		for(size_t j = i + 1; j < GetMethods().size(); j++)
		{
			if (GetMethods()[j] == GetMethods()[i])
				return (false);
		}
	}
	return (true);
}

void	Location::fill_locationInfos(std::ifstream &config_file, const std::string& line, int& braceBalance)
{
	std::string	location_line;
	std::string value;
	std::string word2;

	std::istringstream iss(line);
	std::string LocationWord;

	iss >> LocationWord;
	
	if (!LocationWord.compare("location"))
		GetPath_location(line);
	while (getline(config_file, location_line))
	{
		location_line = trim_white_spaces(isComment(location_line));
		
		if (!location_line.empty())
		{
			std::istringstream iss(location_line);
			std::string keyWord;

			if (!(iss >> keyWord))
				throw std::runtime_error("Error: Invalid key in Server config for " + keyWord);
			if (ValidKey(keyWord) && keyWord != "{")
				throw std::runtime_error("Error: Invalid key in Server config for " + keyWord);
			
			
			std::string word;
			
			if (!location_line.compare("}"))
			{
				CheckBrace(location_line, braceBalance);
				break ;
			}

			if (iss >> word)
			{
				value = word;
				
				if (iss >> word2 && keyWord != "methods")
					throw std::runtime_error("Error: Too many values found for " + keyWord + " in location context.");
			} 
			else
			{
				if (keyWord == "{")
					throw std::runtime_error("Error: Unmatched opening brace '{'");
				throw std::runtime_error("Error: No value found for " + keyWord + " in Server config. " + gai_strerror(errno));

			}

			if (!keyWord.compare("root"))
			{
				if (value[0] != '/' && value[0] != '.')
					value.insert(0, "/");
				this->SetRoot(value);
			}
			else if (!keyWord.compare("methods"))
			{
				std::vector<std::string>	methods;
				methods.push_back(value);
				if (!word2.empty())
					methods.push_back(word2);
				while (iss >> word)
					methods.push_back(word);
				
				for (size_t i = 0; i < methods.size(); ++i)
				{
					if (!(!methods[i].compare("GET") || !methods[i].compare("POST") || !methods[i].compare("DELETE")))
						throw std::runtime_error("Error: Invalid value " + methods[i] + " for methods in location context.");
					
				}
				this->SetMethods(methods);
				if (!DuplicateMethod())
					throw std::runtime_error("Error: Duplicated Methods found.");
			}
			else if (!keyWord.compare("directory_listing"))
			{
				if (value == "on")
					this->SetDirectoryListing(true);
				else if (value == "off")
					this->SetDirectoryListing(false);
				else
					throw std::runtime_error("Error: Invalid value for directory_listing in location context.");
			}
			else if (!keyWord.compare("default_file"))
				this->SetDefaultFile(value);
			else if (!keyWord.compare("cgi"))
			{
				CheckBrace(location_line, braceBalance);
				this->SetCGI(fill_map(config_file, keyWord, braceBalance));
				
				std::map<std::string, std::string>::const_iterator iter;
				for(iter = this->_Cgi.begin(); iter != this->_Cgi.end(); iter++)
				{
					if (iter->first != "php" && iter->first != "py")
						throw std::runtime_error("Error: Unsoported extension: " + iter->first + " for " + " CGI.");
					// if (!CheckFile(iter->second))
					// 	throw std::runtime_error("Error: Invalid CGI path: " + iter->second + " for " + iter->first);
				}
				
			}
			else if (!keyWord.compare("file_upload"))
			{
				CheckBrace(location_line, braceBalance);
				this->SetFileUpload(fill_map(config_file, keyWord, braceBalance));
			
			}
			else if (!keyWord.compare("redirect"))
			{
				if (value[0] != '/' && value[0] != '.')
					value.insert(0, "/");
				this->SetRedirect(value);
			}
			else if (!keyWord.compare("directory"))
			{
				if (value[0] != '/' && value[0] != '.')
					value.insert(0, "/");
				this->SetDirectory(value);
			}
		}
	}
}
