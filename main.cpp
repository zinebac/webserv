#include "Headers/WebServerIncludes.hpp"

int main(int argc, char **argv) {
    srand(static_cast<unsigned int>(time(nullptr))); // check if allowed
    ft_signal();
    try {
       	std::vector<ServerConfig> servers;
		ConfigFile conf(servers);
        if (argc == 2)
		    conf.Check_Conf_file(argv[1]);
        else
            conf.Check_Conf_file("zait-che.conf");
        httpServer test(conf.GetServersConfig());
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}