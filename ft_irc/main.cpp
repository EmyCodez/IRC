# include "Server.hpp"
# include <iostream>
# include <signal.h>
# include <vector>
# include <string>

int main(int argc, const char **argv)
{

    if (argc != 3)
    {
        std::cout << "usage: ./ircserv <port> <password>" <<std::endl;
        return (1);
    }
    try
    {

    } catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
    return (0);
}
