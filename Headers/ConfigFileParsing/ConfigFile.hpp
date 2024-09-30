/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 11:47:37 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/04 21:26:53 by gkarib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
#define PARSING_HPP

#include "../WebServerIncludes.hpp"

enum
{
	SERVER,
	LOCATION,
	ERROR_PAGES,
	ERROR
};

class   ConfigFile
{
	private:
		std::ifstream				_Config_file;
		std::vector<ServerConfig>	_Servers;

	public:
		ConfigFile();
		~ConfigFile();

		ConfigFile (const ConfigFile& copy);
		ConfigFile& operator=(const ConfigFile& copy);
		
		ConfigFile(std::vector<ServerConfig>& servers) : _Servers(servers) {} // !understand
		

		// --------------------------------- GETTERS ---------------------------------
		
		// std::ifstream					GetConfigFile( void ); // *std::ifstream objects are non-copyable, meaning they cannot be copied or assigned.
		const std::vector<ServerConfig>& GetServersConfig( void )	const;
		
		// --------------------------------- SETTERS ---------------------------------
		
		void	SetServers(std::vector<ServerConfig> Server);
		
		// --------------------------------- OTHER FUNCTIONS ---------------------------------

		bool	open_file(const char *str);
		void	read_ConfigFile( void );
		void    Check_Conf_file(const char *str);
		void	ResolvableHost_Port(ServerConfig currentServer);
		bool    isServerAdded(const ServerConfig& server);
		void	clearLocation( void );



};


// ############################	PARSING CONFIG FILE	############################ 

bool		check_extension(const char *str, const char *extension);
std::string	trim_white_spaces(std::string line);
std::string	isComment(std::string line);
int			DefineContext(std::string	line);
bool		CheckFile(std::string file_name);
bool 		CheckDirectory(const std::string& path);
bool		CheckBrace(const std::string& line, int& braceBalance);

#endif