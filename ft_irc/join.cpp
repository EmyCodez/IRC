# include "Server.hpp"
# include "Client.hpp"

void join(Server *server, Client *client, std::vector<std::string> const &params)
{
     if(params.empty())
     {
        client->write(":ft_irc.server 461 " + client->getNickName() + "Not enough parameters \r\n");
        return;
     }
     
}