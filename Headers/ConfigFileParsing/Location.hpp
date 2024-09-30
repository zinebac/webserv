/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ghita <ghita@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 21:25:17 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/08 03:23:46 by ghita            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONPARSER_HPP
#define LOCATIONPARSER_HPP

#include "../WebServerIncludes.hpp"

class Location
{
	private : 
		std::string							_Path;
		std::string							_Root;
		std::string							_Index;
		std::vector<std::string>			_Methods;
		bool								_DirectoryListing;
		std::string							_DefaultFile;
		std::map<std::string, std::string>	_Cgi;
		std::map<std::string, std::string>	_FileUpload;
		std::string							_Redirect;
		std::string							_Directory;

	public :
		Location();
		~Location();

		Location (const Location& other);
		Location&  operator=(const Location& other);
		
		// --------------------------------- GETTERS ---------------------------------
		
		std::string					    	GetPath( void ) const;
		std::string					    	GetRoot( void ) const;
		std::string					    	GetIndex( void ) const;
		std::vector<std::string>	        GetMethods( void ) const;
		bool						        GetDirectoryListing( void ) const;
		std::string					        GetDefaultFile( void ) const;
		std::map<std::string, std::string>	GetCGI( void ) const;
		std::string					        GetRedirect( void ) const;
		std::string					        GetDirectory( void ) const;
		std::map<std::string, std::string>  GetFileUpload( void ) const;
		
		// --------------------------------- SETTERS ---------------------------------
		
		void	SetPath(const std::string& path);
		void	SetRoot(const std::string& root);
		void	SetIndex(const std::string& root);
		void	SetMethods(const std::vector<std::string>& _Methods);
		void	SetDirectoryListing(bool __DirectoryListing);
		void	SetDefaultFile(const std::string& _DefaultFile);
		void	SetCGI(const std::map<std::string, std::string>& _Cgi);
		void	SetRedirect(const std::string& _Redirect);
		void	SetDirectory(const std::string& _Directory);
		void	SetFileUpload(const std::map<std::string, std::string>& _FileUpload);

		// --------------------------------- OTHER FUNCTIONS ---------------------------------

		void	clearLocation( void );
		bool	isLocationConfigFilled( void );
		bool	ValidKey(std::string key);
		void	GetPath_location(const std::string& line);
		bool	DuplicateMethod( void );
		void	fill_locationInfos(std::ifstream &config_file, const std::string& line, int& braceBalance);
};

std::ostream&	operator<<(std::ostream& os, const Location& location);

#endif