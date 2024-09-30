#include "../Headers/WebServerIncludes.hpp"

/*----------------- pars ------------------*/

pars::pars()
{
    // err = 0;
    status = 0;
    cgi_serv = 0;
    // bounce_client = 0;
    changeEvent = 0;
    end_flag = -1;
    t_valread = 0;
    valread = 0;
    cont_l = 0;
    // max = M_H;
    s = 0;
    chunk_n = -1;
    to_be_skip = 1;
    written = 0;
    to_write = 0;
    f_open = 0;
    fdout = 0;
    hex_l = 0;
    cont_l = 0;
    up_fd = -1;
}

pars::pars(pars const & obj) {
	*this = obj;
}

pars& pars::operator=(pars const & obj) {
	if (this != &obj) {
		fdout = obj.fdout;
		chyata = obj.chyata;
		// err = obj.err;
		changeEvent = obj.changeEvent;
		ex = obj.ex;
		ful_path = obj.ful_path;
		cgi_serv = obj.cgi_serv;
		status = obj.status;
		cgi_file = obj.cgi_file;
		// bounce_client = obj.bounce_client;
		end_flag = obj.end_flag;
		written = obj.written;
		to_write = obj.to_write;
		hex_l = obj.hex_l;
		to_be_skip = obj.to_be_skip;
		hexa = obj.hexa;
		hexa = obj.hexa;
		chunk_n = obj.chunk_n;
		// type = obj.type;
		t_valread = obj.t_valread;
		valread = obj.valread;
		cont_l = obj.cont_l;
		// max = obj.max;
		s = obj.s;
		headers = obj.headers;
		up_fd = obj.up_fd;
		f_open = obj.f_open;
		file_name = obj.file_name;
		// host = obj.host;
	}
	return *this;
}

/*----------------- ClientSocket ------------------*/

clientSocket::clientSocket(int fd) {
	socket_fd = fd;
}

clientSocket::clientSocket(const clientSocket& obj) {
	*this = obj;
}

clientSocket& clientSocket::operator=(clientSocket const & obj) {
	if (this != &obj) {
		p = obj.p;
		socket_fd = obj.socket_fd;
		cgi  = obj.cgi;				
	}
	return *this;
}

void    p_reset(pars &p)
{
    p.changeEvent = 1;
    p.to_write = 0;
    p.written = 0;
    p.f_open = 0;
    p.file_name.clear();
    if (p.up_fd > 0)
        close(p.up_fd);
}

void    write_content(pars &p, size_t len, char *body, ServerConfig &data)
{
        p.s = len;
        write(p.up_fd, body, p.s);
        p.upd_valread(data);
}

void    str_to_hexa(pars& p)
{
    std::istringstream s(p.hexa);
    s >> std::hex >> p.chunk_n;
}

int check_hexa (pars &p, std::string &body)
{
    size_t rn;
    if (p.to_be_skip == 1)
    {
        p.to_be_skip = 0;
        rn = body.find("\n");
        if (rn == std::string::npos)
            throw 400;
        p.hex_l = rn + 1;
        p.hexa += body.substr(0, rn + 1);
        str_to_hexa(p);
        if (!p.chunk_n)
        {
            p_reset(p);
            return (0);
        }
        p.hexa.clear();
        return (1);
    }
    else if (p.to_be_skip == 2)
    {
        p.hex_l = 0;
        p.to_be_skip = 0;
        if ((rn = body.find("\n")) == std::string::npos)
            throw 400;
        p.hexa += body.substr(0, rn + 1);
        p.hex_l += rn + 1;
        body.erase(0, rn + 1);
        if ((rn = body.find("\n")) == std::string::npos)
            throw 400;
        p.hexa += body.substr(0, rn + 1);
        p.hex_l += rn + 1;
        return (1);
    }
    else
    {
        if ((rn = body.find("\r\n")) != std::string::npos)
        {
            if (rn != 0)
                throw 400;
            body.erase(0, 2);
            if ((rn = body.find("\r\n")) != std::string::npos)
            {
                p.hex_l = rn + 4;
                p.hexa = "\r\n" + body.substr(0, rn + 2);
                str_to_hexa(p);
                if (!p.chunk_n)
                {
                    p_reset(p);
                    return (0);
                }
                p.hexa.clear();
                return (1);
            }
            else
            {
                p.hex_l = body.size() + 2;
                p.hexa = "\r\n" + body;
                p.to_be_skip = 1;
                return (1);
            }
        }
        else
        {
            if (body.size() >= 2)
                throw 400;
            p.hex_l = body.size();
            p.hexa = body;
            p.to_be_skip = 2;
            return (1);
        }
    }
}

int rm_hexa(pars &p, std::string &body, ServerConfig& data)
{
    char    *b = const_cast<char *> (body.c_str());
    size_t  stream_size = body.size();

    if (p.to_be_skip)
    {
        p.written = 0;
        std::string tmp(b, stream_size);

        if (!check_hexa(p, tmp))
            return (0);

        stream_size -= p.hex_l;
        b += p.hex_l;
    }
            //********AFTER REMOVIG THE FIRST HEXA


            while (stream_size > 0)
            {
                p.to_be_skip = 0;
                p.to_write = std::min(p.chunk_n - p.written, stream_size);
                // ////std::cout << "streamsize=" << stream_size << std::endl;
                // ////std::cout << "this is to_write " << p.to_write << std::endl;
                // ////std::cout << "because chunk_n=" << p.chunk_n << " - p.written=" << p.written << " =" << p.chunk_n - p.written << std::endl;
                write_content(p, p.to_write, b, data);

                stream_size -= p.to_write;
                    if (stream_size == std::string::npos)
                        throw 500;
                // ////std::cout << "streamsize - to_write ==" << stream_size << std::endl;
                b += p.to_write;

                if (p.to_write + p.written == p.chunk_n)
                {
                    p.written = 0;
                    if (!stream_size)
                    {
                        p.hexa.clear();
                        p.to_be_skip = 2;
                        // ////std::cout << "broke here" << std::endl;
                        break;
                    }
                        // ////std::cout << "writting a part " << std::endl;
                        std::string tmp2(b, stream_size);
                        // ////std::cout << "this is the body before |" << tmp2 << "|" << std::endl;
                        // ////std::cout << "its streamsize is " << stream_size << std::endl;
                        std::string tmp(b, stream_size);
                        if (!check_hexa(p, tmp))
                            break;
                        stream_size -= p.hex_l;
                        if (stream_size == std::string::npos)
                            throw 500;
                        b += p.hex_l; //check later;
                        std::string tmp1(b, stream_size);
                        // ////std::cout << "this is the body now|" << tmp1 << "|" << std::endl;
                        // ////std::cout << "its streamsize - hex_l " << stream_size << std::endl;
                }
                else
                {
                    // ////std::cout << "writting all the streamsize " << std::endl;
                    p.to_be_skip = 0;
                    p.written += p.to_write;
                }
            }
        return (1);
}



void	pars::upd_valread(ServerConfig &data)
{
    t_valread += s;
    // ////std::cout << "witten " << t_valread << std::endl;
    fflush(stdout);
    if (t_valread > data.GetLimitClientBodySize())
    {
        //std::cout << "t_valread=" << t_valread << " M_B=" << M_B << std::endl;
       throw 413;
    }
}

int check_stat(std::string up_path)
{
    //left it here
    struct stat buf;
    ////std::cout << "this is the path that was searched " << up_path << std::endl;
    if (stat(up_path.c_str(), &buf) != 0)
        return (0);
    return (1);
}


int httpServer::check_cgi(pars &p, Location &m_loc, std::string &root)
{
    std::string real_url = p.headers["url"].substr(m_loc.GetPath().size(), std::string::npos);
    std::string ful_path = root + real_url;
    erase_consequtive_slash(ful_path);
    if (!check_stat(ful_path))
        return (404);
    size_t i = real_url.find_last_of(".");
    if (i == std::string::npos)
        return (403);
    std::string ext = real_url.substr(i + 1, std::string::npos);
    ////std::cout << "this is the extension " << ext << std::endl;
    std::map<std::string, std::string> test = m_loc.GetCGI();
    if (test.find(ext) == test.end())
        return (403);
    p.cgi_file  = generateRandomFilename();
    p.ex = m_loc.GetCGI()[ext];
    ////std::cout << m_loc.GetCGI()[ext] << std::endl;
    p.ful_path = ful_path;
    //flag cgi here;
    p.cgi_serv = 1;
    return (200);
}

int httpServer::check_conf_p(clientSocket &client, pars &p, ServerConfig &data)
{
    erase_consequtive_slash(p.headers["url"]);
    Location m_loc = getMatchingLocation(p, data);
    if (p.status == 405)
        return (0);
    if (!m_loc.GetPath().size())
    {
        p.status = 404;
        return (0);
    }

    const std::vector<std::string> &methods = m_loc.GetMethods();
    std::string root = getRoot(data, m_loc);
    erase_consequtive_slash(root);

    if (std::find(methods.begin(), methods.end(), "POST") == methods.end())
    {
        p.status = 405;
        return (0);
    }

    ////std::cout << "this is the enabled ==>" << (m_loc.GetFileUpload().find("enabled") == m_loc.GetFileUpload().end()) << std::endl;
    if (m_loc.GetFileUpload().find("enabled") == m_loc.GetFileUpload().end() || \
    m_loc.GetFileUpload().find("directory") == m_loc.GetFileUpload().end() || \
    m_loc.GetFileUpload()["enabled"] != "true")
    {
        p.status = check_cgi(p, m_loc, root);
        return (0);
    }
    std::string path;
    path = root + m_loc.GetPath(); 
    erase_consequtive_slash(path);

    size_t i_slash = p.headers["url"].substr(m_loc.GetPath().size(), std::string::npos).find_last_of("/");
    // if (i_slash == 0)
    //     i_slash = std::string::npos;
    //std::cout << "this is last slash without trunc " << p.headers["url"].substr(0, i_slash) << std::endl;
    //std::cout << "this is last slash avec trunc " << p.headers["url"].substr(m_loc.GetPath().size(), i_slash) << std::endl;
    std::string last_slash = root + p.headers["url"].substr(m_loc.GetPath().size(), i_slash) + "/";
    erase_consequtive_slash(last_slash);

    std::string ful_file_upload = m_loc.GetFileUpload()["directory"] + "/";
    erase_consequtive_slash(ful_file_upload);
    //std::cout << "thiis is the last_slash path" << last_slash << std::endl;
    //std::cout << "thiis is the ful_file_upload path" << ful_file_upload << std::endl;
    if (ful_file_upload == last_slash)
    {
        if (!check_stat(ful_file_upload))
        {
            p.status = 500;
            return (0);
        }
        int f = check_stat(root + p.headers["url"].substr(m_loc.GetPath().size(), std::string::npos));
        if (f == 0)
        {
            p.file_name = root + p.headers["url"].substr(m_loc.GetPath().size(), std::string::npos);
            p.status = 201;
            return (0);
        }
        else
        {
            //std::cout << "file already exists! " << std::endl;
            p.status = check_cgi(p, m_loc, root);//check cgi
            return (0);
        }
    }
    else
    {
        //std::cout << "went to check cgi" << std::endl;
        p.status = check_cgi(p, m_loc, root);//check cgi
        return (0);
    }
    (void)client;
}

void    httpServer::handle_cgi(clientSocket &client, ServerConfig &data)
{
    if (client.p.cgi_serv)
    {
        int state;
        //std::cout << "this is the ful path " << client.p.ful_path << std::endl;
        if (client.cgi.GetExecutedCgi())
        {
            try{
                client.cgi.ExecuteCGI(client.p);
            }
            catch (...)
            {
                throw 500;
            }
            state = client.cgi.GetCgiStatus();
            if (!state)
            {
                //std::cout << "\n*******yup its dead***\n" << std::endl;
                client.p.status = 200;
                //std::cout << "this is the status ==> " << client.p.status << std::endl;
                setExtension(client.p.ful_path, client.p);
                if (client.p.ex == ".php")
                    generateResponse(parseCgiOutput(getContent(client.cgi.GetOutfile(), client, data), client, data),client);
                else
                    generateResponse(getContent(client.cgi.GetOutfile(), client, data), client);
                std::remove(client.cgi.GetOutfile().c_str());
                std::remove(client.p.cgi_file.c_str());
            }
        }
        else
        {
            Cgi &_cgi = client.cgi;
            _cgi.SetExecutablePath(client.p.ex);
            client.p.ex.clear();
            _cgi.SetScriptPath(client.p.ful_path);
            _cgi.SetBodyFileName(client.p.cgi_file);
            _cgi.SetOutfile(generateRandomFilename());
            try {
                _cgi.ExecuteCGI(client.p);
            }
            catch(...) {
                throw 500;
            }
            state = client.cgi.GetCgiStatus();
            if (!state)
            {
                client.p.status = 200;
                //std::cout << "this is the status ==> " << client.p.status << std::endl;
                setExtension(client.p.ful_path, client.p);
                if (client.p.ex == ".php")
                    generateResponse(parseCgiOutput(getContent(client.cgi.GetOutfile(), client, data), client, data),client);
                else
                    generateResponse(getContent(client.cgi.GetOutfile(), client, data), client);
                std::remove(client.cgi.GetOutfile().c_str());
                std::remove(client.p.cgi_file.c_str());
            }
        }
    }
    else
    {
        //std::cout << "generating the normal response " << std::endl;
        //std::cout << "this is the status ==> " << client.p.status << std::endl;
        //check the headers;
        std::map<int, std::string> err = data.GetErrorPages();
        if (err.find(client.p.status) != err.end())
        {
            //std::cout << "found it" << std::endl;
            generateResponse( getContent(err[client.p.status], client, data),client);
        }
        else
        {
            //std::cout << "generating custom page" << std::endl;
            generateResponse( generateDynamicPage(client.p.status, client.p),client);
        }
    }
}

void    httpServer::POST(std::string body, clientSocket &client, ServerConfig &data)
{
    try
    {
        //std::cout << "Went to POST with this url " << client.p.headers["url"] << std::endl;
        if (client.p.changeEvent)
        {
            handle_cgi(client, data);
            return ;
        }
        if (!client.p.f_open && !check_conf_p(client, client.p, data))
        {
            // //std::cout << "this is the exit status ==>" << p.status << std::endl;
            if (client.p.status < 200 || client.p.status > 299)
            {
                p_reset(client.p);
                // //std::cout << "\n\n\n**********set 0******yalatif\n\n\n" << std::endl;
                //std::cout << "heeeeh this is the status " << client.p.status << std::endl;
                return ;
            }
            ////std::cout << "this is the filename" << p.file_name << std::endl;
            if (client.p.cgi_serv)
                client.p.file_name = client.p.cgi_file;
            //std::cout << "openning the input_file==>" << client.p.file_name << std::endl;
            client.p.up_fd = open(client.p.file_name.c_str(), O_RDWR | O_CREAT, 777);
            client.p.f_open = 1;
            if (client.p.up_fd <= 0)
            {
                perror("failed creating file");
                ////std::cout << "this is the fd " << p.up_fd << std::endl;
                throw 500;
            }

            if (client.p.headers.find("Content-Length") != client.p.headers.end())
            {
                if (client.p.headers["Content-Length"].find_first_not_of(" 0123456789") != std::string::npos)
                    throw 400;
                std::istringstream s(client.p.headers["Content-Length"]);
                s >> client.p.cont_l;
                if (client.p.cont_l > data.GetLimitClientBodySize())
                    throw 413;
            }
            ////std::cout << "this is the content_l " << p.cont_l << std::endl;
        } 
        /********/

        if (client.p.headers.find("Transfer-Encoding") != client.p.headers.end() && client.p.headers["Transfer-Encoding"] == "chunked")
            rm_hexa(client.p, body, data);

        else
        {
            client.p.s = body.size();
            if (client.p.t_valread <= client.p.cont_l)
            {
                // p.upload_file.write(body.c_str(), p.s);
                // p.upload_file.flush();
                write(client.p.up_fd ,body.c_str(), client.p.s);
                client.p.upd_valread(data);
            }
                if (client.p.t_valread == client.p.cont_l) 
                {
                ////std::cout << "\n\n\n****************"  << "sf ra salat" << "*********" << std::endl;
                fflush(stdout);
                // p.upload_file.close();
                //std::cout << "\n\n\n**********set 0******\n\n\n" << std::endl;
                p_reset(client.p);
                }
        }
    }
    catch (int s)
    {
        client.p.status = s;
        p_reset(client.p);
        client.p.ex.clear();
        client.p.cgi_serv = 0;
    }
    (void)data;
}