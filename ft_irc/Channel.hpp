# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <map>

# include "Ft_Irc.hpp"
# include "Client.hpp"

class Channel
{
private:
    std::string _name;
    std::vector<Client *> _clientList;
    std::vector<Client *> _inviteList;
    std::map<std::string, bool> _operators;
    std::string _channelKey;
    std::string _topic;
    size_t _userLimit;

    Channel();
    Channel(const Channel &other);
    Channel &operator= (const Channel &other);

public:
    Channel(std::string name, std::string password);
    ~Channel();
};

# endif