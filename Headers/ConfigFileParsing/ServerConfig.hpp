/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ghita <ghita@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 21:27:09 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/07 20:17:16 by ghita            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERPARSER_HPP
#define SERVERPARSER_HPP

#include "../WebServerIncludes.hpp"

class  ServerConfig
{
	private:
		std::string					_Host;
		int							_Port;
		std::string					_Root;
		std::string					_ServerNames;
		bool						_DefaultServer;
		std::map<int, std::string>	_ErrorPages;
		int							_LimitClientBodySize;
		std::vector<Location>		_Locations;
	
	public: 
		ServerConfig();
		~ServerConfig();

		ServerConfig(const ServerConfig& other);
		ServerConfig& operator=(const ServerConfig& other);
		
		// --------------------------------- GETTERS ---------------------------------

		std::string						GetHost( void ) const;
		int								GetPort( void ) const;
		std::string				    	GetRoot( void ) const;
		std::string						GetServerNames( void ) const;
		bool							GetDefaultServer( void ) const;
		std::map<int, std::string>		GetErrorPages( void ) const;
		int								GetLimitClientBodySize( void )  const;
		std::vector<Location>    		GetLocation( void ) const;
		
		// --------------------------------- SETTERS ---------------------------------

		void    SetHost(const std::string& host);
		void    SetPort(int port);
		void    SetRoot(const std::string& root);
		void    SetServerNames(const std::string& serverName);
		void    SetDefaultServer(bool defaultServer);
		void    SetErrorPages(const std::map<int, std::string>& errorPages);
		void    SetLimitClientBodySize(int limitClientBodySize);
		void    SetLocation(const std::vector<Location>& location);
		
		// --------------------------------- OTHER FUNCTIONS ---------------------------------
		
		void    fill_ServerInfos(const std::string& line, std::ifstream& config_file, int &braceBalance);
		std::map<int, std::string>	fill_ErrorPages(std::ifstream& config_file, int &braceBalance);
		bool    isServerConfigFilled();
		void    clearServerConfig();
		bool	ValidKey(std::string line);

};

std::ostream& operator<<(std::ostream& os, const ServerConfig& server);

#endif
