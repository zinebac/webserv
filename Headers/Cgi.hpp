/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 17:29:48 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/05 21:36:19 by gkarib           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WebServerIncludes.hpp"

class pars;
class Cgi
{
	private:
		pid_t                    _CgiPid;
		std::vector<std::string> _EnvCgi;
		std::string              _ExecutablePath; // from the config file // "/Users/gkarib/Desktop/CGI/cgi-bin/php-cgi";
		std::string              _ScriptPath; // the file that we r going to execuute // "/Users/gkarib/Desktop/CGI/cgi-bin/php-script.php"
		int                      _InputFile;
		int                      _OutputFile; // this is the file descriptor i am supposed to read from
		std::string              _BodyFileName; // to open the file
		std::string              _Outfile; // if this is the output file path i am going to need it to check the content lenght
		int                      _CgiStatus; // Flag to check if the cgi finished or not
		int						_ExecutedCgi; // flag to check if the cgi started or not

	public:
		Cgi();
		~Cgi();
		Cgi &operator=(const Cgi &cgi);
		
	// --------------------------------- GETTERS ---------------------------------
		
		char    	**GetCGIEnv( void );
		int     	GetInputFile( void );
		int     	GetOutputFile( void );
		char   		*GetExecutablePath( void );
		char    	*GetScriptPath( void );
		int			GetCgiStatus( void );
		int			GetExecutedCgi( void );
		std::string	GetOutfile( void );
		int			GetCgiPid();
	
	// --------------------------------- SETTERS ---------------------------------

		void    SetCGIEnv( char **cgi_env );
		void    SetExecutablePath(std::string executable_path);
		void    SetScriptPath(std::string script_path);
		void    SetInputFile(int input_file);
		void    SetOutputFile(int output_file);
		void    SetBodyFileName(std::string body_file_name);
		void    SetOutfile(std::string outfile);


	// --------------------------------- OTHER FUNCTIONS --------------------------------   
	
		void    FillCgiEnv(pars &p);
		void	ExecuteCGI(pars &p);
		
		
	
};

#endif