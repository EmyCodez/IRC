# ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include <iostream>
# include <signal.h>
# include <vector>
# include <map>
# include <poll.h>

class Server
{

    private:
        int  _port;
        int _listenSocket;
        std::string _password;
        std::vector<struct pollfd> _fds;
        std::map<int, Client *> _clients;
        Server();
        Server(const Server &other);
        Server &operator= (const Server &other);

    public:
        Server(const std::string &port, std::string &password);
        ~Server();

        static bool stop;
        static void handleSignal(int signal);

        // member functions
        void start();  
        void acceptConnection();
        void readFromClient(int fd);
        void disconnectClient(int fd, const std::string reason);

        //getters
        std::string getPassword() const { return _password; }
        Client *getClientByNickName(const std::string &nickname);  
};

# endif