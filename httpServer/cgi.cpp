/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkarib <gkarib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/08/09 01:59:06 by heloufra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/WebServerIncludes.hpp"

Cgi::Cgi()
{
	_EnvCgi = std::vector<std::string>();
	_ExecutablePath = "";
	_ScriptPath = "";
	_InputFile = -1;
	_OutputFile = -1;
	_BodyFileName = "";
	_Outfile = "";
	_CgiStatus = 0;
	_ExecutedCgi = 0;
}

Cgi::~Cgi()
{
	if (_EnvCgi.size() > 0)
	{
    	char **cgiEnv = GetCGIEnv();
		delete[] cgiEnv;
	}
}

Cgi &Cgi::operator=(const Cgi &copy)
{
	this->_CgiPid = copy._CgiPid;
	this->_EnvCgi = copy._EnvCgi;
	this->_ExecutablePath = copy._ExecutablePath;
	this->_ScriptPath = copy._ScriptPath;
	this->_InputFile = copy._InputFile;
	this->_OutputFile = copy._OutputFile;
	this->_BodyFileName = copy._BodyFileName;
	this->_Outfile = copy._Outfile;
	this->_CgiStatus = copy._CgiStatus;
	this->_ExecutedCgi = copy._ExecutedCgi;
	return (*this);
}


// --------------------------------- GETTERS ---------------------------------

char **Cgi::GetCGIEnv()
{
	char **cgi_env = new char*[_EnvCgi.size() + 1];
	for (size_t i = 0; i < _EnvCgi.size(); ++i)
		cgi_env[i] = const_cast<char *>(_EnvCgi[i].c_str());
	cgi_env[_EnvCgi.size()] = NULL;
	
	 return (cgi_env);
}

int Cgi::GetCgiPid()
{
	return (_CgiPid);
}

int Cgi::GetInputFile()
{
	return (_InputFile);
}

int Cgi::GetOutputFile()
{
	return (_OutputFile);
}

char *Cgi::GetExecutablePath()
{
	return (const_cast<char *>(_ExecutablePath.c_str()));
}

char *Cgi::GetScriptPath()
{
	return(const_cast<char *>(_ScriptPath.c_str()));
}

int    Cgi::GetCgiStatus( void )
{
	return (_CgiStatus);
}

int    Cgi::GetExecutedCgi( void )
{
	return (_ExecutedCgi);
}

std::string   Cgi::GetOutfile( void )
{
    return _Outfile;
}

// --------------------------------- SETTERS ---------------------------------

void Cgi::SetCGIEnv( char **cgi_env )
{
	_EnvCgi.clear();
	for (size_t i = 0; cgi_env[i] != NULL; ++i)
		_EnvCgi.push_back(cgi_env[i]);
}

void    Cgi::SetExecutablePath(std::string executable_path)
{
    this->_ExecutablePath = executable_path;
}

void    Cgi::SetScriptPath(std::string script_path)
{
    this->_ScriptPath = script_path;
}

void    Cgi::SetInputFile(int input_file)
{
    this->_InputFile = input_file;
}

void    Cgi::SetOutputFile(int output_file)
{
    this->_OutputFile = output_file;
}

void    Cgi::SetBodyFileName(std::string body_file_name)
{
    this->_BodyFileName = body_file_name;
}

void    Cgi::SetOutfile(std::string outfile)
{
    this->_Outfile = outfile;
}

// --------------------------------- OTHER FUNCTIONS --------------------------------   

void    Cgi::FillCgiEnv(pars &p)
{
	
	_EnvCgi.clear();

	std::stringstream length;
	std::stringstream content_type;
	httpServer obj;
	_EnvCgi.push_back("PATH_INFO=" + p.headers["url"]);
	_EnvCgi.push_back("QUERY_STRING=" + p.headers["query"]);
	_EnvCgi.push_back("HTTP_COOKIE=" + p.headers["Cookie"]); // cookies
	_EnvCgi.push_back("REQUEST_METHOD=" + p.headers["method"]);
	_EnvCgi.push_back("REDIRECT_STATUS=200");
	length << p.to_write;
	_EnvCgi.push_back("CONTENT_LENGTH=" + ((p.headers.find("Content-Length") != p.headers.end()) ? p.headers["Content-Length"] : length.str())); // check this if valid
	_EnvCgi.push_back("SCRIPT_FILENAME=" + _ScriptPath);
	_EnvCgi.push_back("SERVER_PROTOCOL=" + p.headers["httpversion"]);
	_EnvCgi.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_EnvCgi.push_back("CONTENT_TYPE=" + p.headers["Content-Type"]); // hna ta7 nda
}

void	Cgi::ExecuteCGI(pars &p)
{
	int status;
	pid_t pid;
	if (!_ExecutedCgi) {
	    _ExecutedCgi = 1;

	    char *argv[3] = {GetExecutablePath(), GetScriptPath(), NULL};

	    if (p.headers["method"] == "POST") {
	        _InputFile = open(_BodyFileName.c_str(), O_RDONLY, 0777);
	        if (_InputFile < 0)
	            throw std::runtime_error("Error opening the InputFile");
	    }

	    _OutputFile = open(_Outfile.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
	    if (_OutputFile < 0)
	        throw std::runtime_error("Error opening the OutputFile");

	     _CgiPid = fork();

	    if (_CgiPid < 0)
	        throw std::runtime_error("Error: Fork failed");
		else if (!_CgiPid)
		{
	        if (p.headers["method"] == "POST") {
	            dup2(_InputFile, STDIN_FILENO);
	            close(_InputFile);
	        }

	        dup2(_OutputFile, STDOUT_FILENO);
	        dup2(_OutputFile, STDERR_FILENO);
	        close(_OutputFile);
			FillCgiEnv(p);
	        execve(GetExecutablePath(), argv, GetCGIEnv());
			exit(1);
		}	
		else
		{
			if (p.headers["method"] == "POST")
				close(_InputFile);
			pid = waitpid(_CgiPid, &status, WNOHANG);
			if (pid == 0)
				_CgiStatus = 1;
			else if (pid == _CgiPid) {
				if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
					throw std::runtime_error("Error: CGI execution failed");
				_CgiStatus = 0;
			}
			add_pid(_CgiPid);
		}
	}
	else
	{
		pid = waitpid(_CgiPid, &status, WNOHANG);
		if (pid == 0)
			_CgiStatus = 1;
		else if (pid == _CgiPid) {
			if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
				throw std::runtime_error("Error: CGI execution failed");
			_CgiStatus = 0;
		}
	}
}