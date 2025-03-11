# include "Server.hpp"
# include "Utils.hpp"
# include <sys/socket.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sstream>

bool Server::stop = false;

Server::Server(const std::string &port, std::string &password) : _listenSocket(-1), _password(password)
{
    try
    {
        std::istringstream(port) >> _port;
    }
    catch (std::exception &e)
    {
        throw std::runtime_error("Inavlid port");
    }
}

Server::Server(const Server &other)
    : _port(other._port), _listenSocket(other._listenSocket), _fds(other._fds) {}

Server::~Server()
{
    if(_listenSocket != -1)
    {
        close(_listenSocket);
        _listenSocket = -1;
    }

    std::map<int, Client *> ::iterator client_it = _clients.begin();
    while (client_it != _clients.end())
    {
        delete client_it->second;
        close(client_it->first);
        ++client_it;
    }
}

Server &Server::operator= (const Server &other)
{
    if(this != &other)
    {
        this->_port = other._port;
        this->_listenSocket = other._listenSocket;
        this->_fds = other._fds;
    }
    return *this;
}

void Server::start()
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    //create socket
    _listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenSocket < 0)
        throw std::runtime_error("Could not create socket");

    // set socket options
    const int enable = 1;
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw std::runtime_error("Could not set socket options");
    else if (fcntl(_listenSocket,F_SETFL, O_NONBLOCK) < 0)  
        throw std::runtime_error("Could not set socket options");      

    // Bind the socket
    if (bind(_listenSocket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error ("Failed to bind socket");

    // Listen for incoming connections
    if (listen(_listenSocket, 10) < 0)
        throw std::runtime_error ("Failed to listen for incoming connections");

    struct pollfd fd = {
                            .fd = _listenSocket,
                            .events = POLLIN,
                            .revents = 0
                        };
    _fds.push_back(fd);

    std::cout<< "Server started on port "<< _port <<std::endl;
    while (Server::stop == false)
    {
       if( poll(_fds.data(),  _fds.size(), -1) < 0 && Server::stop == false)
        throw std::runtime_error ("Poll failed");

        for (size_t i =0; i < _fds.size(); i++)
        {
            int polled_fd = _fds[i].fd;

            if(_fds[i].revents & POLLHUP)
                disconnectClient(polled_fd, "");
            else if (_fds[i].revents & POLLIN)    
            {
                if(polled_fd = _listenSocket)
                    acceptConnection();
                else 
                    readFromClient(polled_fd);    
            }
        }
    }
    std::cout << "Server shut down"<< std::endl;
                                    
}

void Server::acceptConnection()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int fd = accept(_listenSocket, reinterpret_cast<struct sockaddr *> (&addr), &len);
    if(fd < 0)
        throw std::runtime_error("Failed to accept connection");

     std::string welcome = ":ft_irc.server **** Welcome ...\r\n" ;
     send(fd,welcome.c_str(), welcome.size(), 0);

     if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Could not set socket options");
    struct pollfd pfd = {
                            .fd = fd,
                            .events = POLLIN,
                            .revents = 0
                        };
    std::string inet_addr = inet_ntoa(addr.sin_addr);
    Client *client = new Client(fd, inet_addr);
    _fds.push_back(pfd);
    _clients.insert(std::pair<int, Client*>(fd,client));                        
}

void Server::readFromClient(int fd)
{
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    ssize_t bytes = recv(fd,buffer,sizeof(buffer)-1,0);
    if(bytes > 0)
    {
        buffer[bytes]= '\0';
        try
        {
            std::vector<std::string> args = split(buffer,'\n');
            std::vector<std::string> :: iterator it = args.begin();
            while( it != args.end())
            {
                std::string line = *it;
                Client *client = _clients[fd];
                ++it;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
}
