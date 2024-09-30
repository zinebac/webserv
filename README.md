# WebServ

WebServ is a custom HTTP server implementation developed as a team project. This server is designed to handle HTTP requests, demonstrating core concepts of web server functionality, network programming, and HTTP protocol implementation.

![Project Grade](src/media/grade.PNG)

## Table of Contents

1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Technology Stack](#technology-stack)
4. [Installation](#installation)
5. [Usage](#usage)
6. [Project Structure](#project-structure)
7. [Key Concepts](#key-concepts)
8. [Team Contributions](#team-contributions)
9. [Resources and Further Reading](#resources-and-further-reading)
10. [License](#license)

## Project Overview

WebServ is a lightweight HTTP server that implements core functionalities of the HTTP/1.1 protocol. It's designed to handle web serving tasks, demonstrating the fundamental principles of network programming, concurrent connections handling, and HTTP request/response cycle management.

## Features

- HTTP/1.1 protocol support
- GET, POST, and DELETE method handling
- Static file serving
- Custom error pages
- CGI support
- Connection multiplexing using poll()
- Config file parsing for server setup
- Multiple virtual servers support

## Technology Stack

- Language: C++
- Network Programming: POSIX sockets
- I/O Multiplexing: poll() system call
- Build System: Makefile
- CGI: PHP and Python support

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/your-username/webserv.git
   ```
2. Navigate to the project directory:
   ```
   cd webserv
   ```
3. Compile the project:
   ```
   make
   ```

## Usage

1. Run the server with a configuration file:
   ```
   ./webserv <config_file>
   ```
   Example configuration files are provided in the `config_files/` directory.

2. Access the server through a web browser or tools like curl:
   ```
   curl http://localhost:<port>
   ```
3. To stop the server, use Ctrl+C or any interrupting signal.

4. To clean up compiled files:
   ```
   make fclean
   ```

## Project Structure

```
webserv/
├── Headers/
│   ├── Cgi.hpp
│   ├── Libraries.hpp
│   ├── WebServerIncludes.hpp
│   └── ConfigFileParsing/
│       ├── ConfigFile.hpp
│       ├── Location.hpp
│       └── ServerConfig.hpp
├── Parsing/
│   ├── Location.cpp
│   ├── ServerConfig.cpp
│   ├── Utils.cpp
│   ├── config_file.cpp
│   └── overload_operator.cpp
├── config_files/
│   ├── aababach.conf
│   ├── gkarib.conf
│   └── zait-che.conf
├── httpServer/
│   ├── Network.cpp
│   ├── cgi.cpp
│   ├── generators.cpp
│   ├── getters.cpp
│   ├── helpers.cpp
│   ├── httpServer.cpp
│   ├── post.cpp
│   └── setters.cpp
├── src/
│   ├── favicon.ico
│   ├── cgi/
│   │   ├── cookie_setter.php
│   │   ├── counter.php
│   │   ├── error_py.py
│   │   ├── helloworld.php
│   │   ├── helloworld.py
│   │   ├── infinite_loop.php
│   │   ├── info.php
│   │   └── query.php
│   ├── cgi_bin/
│   │   ├── php-cgi
│   │   └── py-cgi
│   ├── errors/
│   │   ├── 400.html
│   │   ├── 403.html
│   │   └── ... (other error pages)
│   ├── home/
│   │   ├── index.html
│   │   ├── lacost.html
│   │   ├── larini.html
│   │   └── ... (other assets)
│   └── media/
│       ├── en.subject.pdf
│       └── notes
├── Makefile
└── README.md
```

## Key Concepts

### Sockets

Sockets are the fundamental building blocks for network communication. In WebServ, we use POSIX sockets to create TCP connections.

### Multiplexing with poll()

The server uses the `poll()` system call and its helper functions to handle multiple client connections concurrently. This allows the server to efficiently manage numerous connections without the need for multi-threading.

### HTTP Protocol

Our server implements key aspects of HTTP/1.1, including:

- Request parsing
- Response formatting
- Header handling
- Status codes
- GET, POST, and DELETE methods

### CGI (Common Gateway Interface)

Our implementation provides CGI support for both PHP and Python, enabling the execution of scripts and returning their output as part of the HTTP response.

### Configuration System

The server uses a custom configuration file system, allowing for easy setup of multiple virtual servers and locations.

## Team Contributions

- **zait-che (Me)**: 
  - GET and DELETE method implementation
  - Request parsing and data structure design
  - Response sending mechanism
  - Multiplexing support
  - Static and error page handling

- **aababach**: 
  - Multiplexing implementation using poll()
  - POST method implementation
  - Custom request parsing and response sending method

- **gkarib**: 
  - Configuration file parsing
  - CGI implementation

## Resources and Further Reading

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [RFC 2616 - HTTP/1.1](https://tools.ietf.org/html/rfc2616)
- [CGI: Common Gateway Interface](https://www.w3.org/CGI/)
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html)
- [NGINX Documentation](https://nginx.org/en/docs/)
- (more helpful links to be added)

## License

This project is licensed under a restrictive, custom license. See the [LICENSE](LICENSE) file for details. Unauthorized use, modification, or distribution of this software is strictly prohibited.