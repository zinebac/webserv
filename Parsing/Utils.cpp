/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ghita <ghita@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/24 21:01:35 by gkarib            #+#    #+#             */
/*   Updated: 2023/08/07 20:14:46 by ghita            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/WebServerIncludes.hpp"

bool    check_extension(const char *str, const char *extension)
{
	if (strlen(str) > strlen(extension))
	{
		if (!strncmp(&str[strlen(str) - strlen(extension)], extension, strlen(extension)))
			return (true);
	}
	throw std::runtime_error("Error: Invalid Extension.");
}

bool	ConfigFile::open_file(const char *file_name)
{
	_Config_file.open(file_name);

	if (!_Config_file)
		throw std::runtime_error(std::string("Error: Can't Open Config File. ") + gai_strerror(errno));
	if (_Config_file.peek() == EOF)
	{
		_Config_file.close();
		throw std::runtime_error(std::string("Error: Empty File. ") + gai_strerror(errno));
	}
	return (true);
}

// bool	CheckFile(std::string file_name)
// {
// 	if (!access(file_name.c_str(), F_OK | X_OK))
// 		return (true);
// 	return (false);
// }
// bool		CheckFile(std::string file_name)
// {
//     struct stat buffer;
//     return (stat(file_name.c_str(), &buffer) == 0);
// }

// #include <sys/stat.h>
// #include <unistd.h>

bool CheckFile(std::string file_name)
{
    struct stat file_info;
    if (stat(file_name.c_str(), &file_info) == 0)
    {
        // Check if the path points to a regular file
        if (S_ISREG(file_info.st_mode))
            return true; // File exists and is a regular file
    }

    return false; // File does not exist or is not a regular file
}


bool CheckDirectory(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return (true);
    }
   return (false);
}

std::string	trim_white_spaces(std::string line)
{
	int	i;
	int	j;

	i = 0;
	j = line.length() - 1;
	while(isspace(line[i]))
		i++;
	while(isspace(line[j]))
		j--;
	return(line.substr(i, j - i + 1));
}

std::string	isComment(std::string line)
{
	size_t found1 = line.find('#');
	size_t found2 = line.find("//");

	if ((found1 != std::string::npos && found2 == std::string::npos) || found1 < found2)
		return (line.substr(0, found1));
	else if (found2 != std::string::npos)
		return (line.substr(0, found2));
	return (line);
}

bool CheckBrace(const std::string& line, int& braceBalance) {
    for (size_t i = 0; i < line.length(); i++) {
        char currentChar = line[i];

        if (currentChar == '{')
		{
            braceBalance++;
            // std::cout << "pushed = " << line << braceBalance << std::endl;
        } 
		else if (currentChar == '}')
		{
            braceBalance--;
            // std::cout << "popped = " << braceBalance << std::endl;
            if (braceBalance < 0)
			{
                throw std::runtime_error("Error: Extraneous closing brace '}'.");
            }
        }
    }

    return true;
}

int	DefineContext(std::string	line)
{
	if (line == "server {")
		return (SERVER);
	if (line == "error_pages: {")
		return (ERROR_PAGES);
	else if (!line.compare(0, 9, "location "))
		return (LOCATION);
	return (ERROR);
}
