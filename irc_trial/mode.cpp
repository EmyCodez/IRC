# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"

/*
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

struct Mode {
    char modeChar;
    bool paramWhenSet;
    bool paramWhenUnset;
};

bool isModeValid(char mode, Mode& outMode) {
    static Mode validModes[] = {
        {'i', false, false},
        {'t', false, false},
        {'k', true,  true},
        {'o', true,  true},
        {'l', true,  false} // l needs param on +, not on -
    };
    const int numModes = sizeof(validModes) / sizeof(validModes[0]);

    for (int i = 0; i < numModes; ++i) {
        if (validModes[i].modeChar == mode) {
            outMode = validModes[i];
            return true;
        }
    }
    return false;
}

void handleMode(Server &server, Client &client, std::vector<std::string> &params) {

    if (params.size() < 2) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
        std::cout << ":" + server.getServerName() + " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n";
        return;
    }

    std::string target = params[0];
    std::string modeStr = params[1];

    Channel *channel = server.getChannel(target);
    if (!channel) {
        client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + target + " :No such channel\r\n");
        return;
    }

    // Remove '+' or '-' from the mode string to process individual modes
    char currentSign = modeStr[0];
    if (currentSign != '+' && currentSign != '-') {
        std::cout << "Invalid mode string. It should start with '+' or '-'.\n";
        return;
    }

    // Start processing each mode in the mode string
    int paramIndex = 2;  // First two params are channel and mode string
    for (std::size_t i = 1; i < modeStr.length(); ++i) {
        char mode = modeStr[i];
        Mode modeInfo;

        if (!isModeValid(mode, modeInfo)) {
            std::cout << "Invalid mode character: " << mode << "\n";
            return;
        }

        if (currentSign == '+') {
            // If setting the mode, check if a parameter is needed
            if (modeInfo.paramWhenSet) {
                if (paramIndex >= params.size()) {
                    std::cout << "Missing parameter for mode: " << mode << "\n";
                    return;
                }
                std::string param = params[paramIndex++];
                std::cout << "Setting mode " << mode << " with parameter: " << param << "\n";
            } else {
                std::cout << "Setting mode " << mode << " (no parameter needed)\n";
            }
        } else if (currentSign == '-') {
            // If unsetting the mode, no parameter is needed
            if (modeInfo.paramWhenUnset) {
                std::cout << "Invalid: Mode " << mode << " should not have a parameter when unsetting.\n";
                return;
            }
            std::cout << "Unsetting mode " << mode << "\n";
        }
    }
}

*/

void handleMode(Server &server, Client &client, std::vector<std::string> &params)
{
    std::cout <<"check>>>>> "<< params[1] << std::endl;
    if(params.empty() || params.size() < 2)
    {
        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
        return;
    }
    std::string target = params[0];
    Channel *channel = server.getChannel(target);
       if(!channel)
       {
            client.write(":"+server.getServerName()+" 403 "+ client.getNickName() + " " + target +  " :No such channel!\r\n");
            return;
       }

    // std::vector <std::string> reqArgs;
    // for (size_t i = 2; i < params.size(); i++)
    //           reqArgs.push_back(params[i]);
   
    // bool isAdded;
    // for (size_t i = 0; i < params[1].size(); i++)
    // {
    //     /*TODO: check for multiple + or - */
    //    switch (params[1][i])
    //    {
    //     case '+':  isAdded = true;
    //                break;
    //     case '-':  isAdded = false;
    //                break;
        // case 'i':  // invite-only channel
        //             if(!channel->isOperator(&client))   
        //             {
        //                 client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
        //                 return;
        //             }  
        //             if(isAdded)
        //                 channel->setInviteOnly(true);
        //             else
        //                 channel->setInviteOnly(false);   
        //             channel->broadcast(":" + client.getPrefix() + " MODE " + target + (isAdded ? "+i" : "-i") + "\r\n");     
        //             break;  
        // case 't':  // set topic
        //             std::cout << "\n in t" << std::endl;
        //             if(isAdded)
        //                 channel->setTopicPrivilege(true);
        //             else
        //                 channel->setTopicPrivilege(false);   
        //             channel->broadcast(":" + client.getPrefix() + " MODE " + target + (isAdded ? "+t" : "-t") + "\r\n");     
        //             break; 
        // case 'o':  // grant or revoke operator privilege
        //             if(reqArgs.empty())
        //             {
        //                 client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
        //                 return;
        //             }
        //             if(!channel->isOperator(&client))   
        //             {
        //                 client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
        //                 return;
        //             }  
        //             Client *targetClient = channel->getClientWithNickname(reqArgs[0]);
        //             if(!targetClient)
        //             {
        //                 client.write(":" + server.getServerName() + " 441 " + client.getNickName() + " " + target + " :They aren't on that channel\r\n");
        //                 return; 
        //             }
        //             if(isAdded)
        //                 channel->addOperator(targetClient);
        //             else
        //                 channel->removeOperator(targetClient);   
        //             channel->broadcast(":" + client.getPrefix() + " MODE " + target + (isAdded ? "+o" : "-o") + "\r\n");  
        //             reqArgs.erase(reqArgs.begin());   
        //             break;                                                     
    //    default: std::cout << "Invalid Mode\r\n";
       
    //    }
    // } 
}




