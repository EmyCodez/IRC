#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6667
#define MAX_CLIENTS 100
#define BUF_SIZE 1024

// IRC Constants
#define SERVER_NAME "MyIRCD"
#define VERSION "1.0"

// Client structure
struct Client {
    int fd;
    std::string nickname;
    std::string username;
    std::unordered_set<std::string> channels;
};

// Channel structure
struct Channel {
    std::string name;
    std::unordered_set<int> users; // Client fds of users in this channel
    std::string topic;
    std::unordered_set<int> operators; // Operators (admin) for the channel
    std::string password; // If channel is password protected
    bool invite_only = false; // If the channel is invite-only
    bool topic_restricted = false; // If only ops can change topic
};

class IRCServer {
public:
    IRCServer() : server_fd(0) {}

    void start() {
        setupServer();
        mainLoop();
    }

private:
    int server_fd;
    struct sockaddr_in server_address;
    std::unordered_map<int, Client> clients;
    std::unordered_map<std::string, Channel> channels;
    struct pollfd fds[MAX_CLIENTS + 1];

    void setupServer() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            std::cerr << "Error creating socket" << std::endl;
            exit(EXIT_FAILURE);
        }

        fcntl(server_fd, F_SETFL, O_NONBLOCK);

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
            std::cerr << "Error binding to port" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_CLIENTS) == -1) {
            std::cerr << "Error listening on socket" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Initialize pollfd
        fds[0].fd = server_fd;
        fds[0].events = POLLIN;
    }

    void mainLoop() {
        while (true) {
            int poll_count = poll(fds, clients.size() + 1, -1);
            if (poll_count == -1) {
                std::cerr << "Poll error!" << std::endl;
                break;
            }

            for (int i = 0; i <= clients.size(); ++i) {
                if (fds[i].revents & POLLIN) {
                    if (fds[i].fd == server_fd) {
                        acceptClient();
                    } else {
                        handleClient(fds[i].fd);
                    }
                }
            }
        }
    }

    void acceptClient() {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "Error accepting client" << std::endl;
            return;
        }

        fcntl(client_fd, F_SETFL, O_NONBLOCK);

        // Add new client to poll() and clients list
        struct pollfd pfd = { client_fd, POLLIN | POLLOUT, 0 };
        fds[clients.size() + 1] = pfd;
        clients[client_fd] = Client{ client_fd, "", "", {} };
    }

    void handleClient(int client_fd) {
        char buffer[BUF_SIZE];
        ssize_t bytes_received = recv(client_fd, buffer, BUF_SIZE, 0);

        if (bytes_received <= 0) {
            close(client_fd);
            clients.erase(client_fd);
            return;
        }

        buffer[bytes_received] = '\0';
        std::string message(buffer);

        std::cout << "Received from client " << client_fd << ": " << message << std::endl;

        // Parse and handle IRC commands
        handleIRCCommand(client_fd, message);
    }

    void handleIRCCommand(int client_fd, const std::string& message) {
        if (message.find("NICK") == 0) {
            setNick(client_fd, message);
        } else if (message.find("USER") == 0) {
            setUser(client_fd, message);
        } else if (message.find("JOIN") == 0) {
            joinChannel(client_fd, message);
        } else if (message.find("PRIVMSG") == 0) {
            sendPrivateMessage(client_fd, message);
        } else if (message.find("KICK") == 0) {
            kickUser(client_fd, message);
        }
        // More commands like INVITE, TOPIC, MODE can be added similarly
    }

    void setNick(int client_fd, const std::string& message) {
        std::istringstream ss(message);
        std::string cmd, nick;
        ss >> cmd >> nick;
        clients[client_fd].nickname = nick;
        sendMessage(client_fd, "NICK " + nick + "\r\n");
    }

    void setUser(int client_fd, const std::string& message) {
        std::istringstream ss(message);
        std::string cmd, username;
        ss >> cmd >> username;
        clients[client_fd].username = username;
        sendMessage(client_fd, "USER " + username + "\r\n");
    }

    void joinChannel(int client_fd, const std::string& message) {
        std::istringstream ss(message);
        std::string cmd, channel_name;
        ss >> cmd >> channel_name;
        clients[client_fd].channels.insert(channel_name);
        channels[channel_name].users.insert(client_fd);
        sendMessage(client_fd, "JOIN " + channel_name + "\r\n");

        // Send a welcome message to the channel
        sendMessageToChannel(channel_name, client_fd, " has joined the channel.");
    }

    void sendPrivateMessage(int client_fd, const std::string& message) {
        // Parse the message and send a private message to a specific user
    }

    void kickUser(int client_fd, const std::string& message) {
        // Parse the message and kick a user from the channel
    }

    void sendMessage(int client_fd, const std::string& msg) {
        send(client_fd, msg.c_str(), msg.length(), 0);
    }

    void sendMessageToChannel(const std::string& channel, int from_fd, const std::string& msg) {
        for (int user_fd : channels[channel].users) {
            if (user_fd != from_fd) {
                sendMessage(user_fd, msg);
            }
        }
    }
};

int main() {
    IRCServer ircServer;
    ircServer.start();
    return 0;
}
