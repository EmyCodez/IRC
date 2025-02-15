#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <poll.h>
#include <vector>

int main()
{
    // Create the listening socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "Can't create a socket!" << std::endl;
        return -1;
    }

    // Bind the socket to IP / port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }

    // Mark the socket for listening
    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!" << std::endl;
        return -3;
    }

    // Prepare pollfd structure to monitor the listening socket
    std::vector<pollfd> fds;
    fds.push_back({listening, POLLIN, 0});

    std::cout << "Server listening on port 54000..." << std::endl;

    while (true)
    {
        int pollCount = poll(fds.data(), fds.size(), -1); // Wait indefinitely for events
        if (pollCount == -1)
        {
            std::cerr << "Poll error!" << std::endl;
            break;
        }

        // Check if there's a new client connection
        if (fds[0].revents & POLLIN)
        {
            sockaddr_in client;
            socklen_t clientSize = sizeof(client);
            char host[NI_MAXHOST];
            char svc[NI_MAXSERV];

            int clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
            if (clientSocket == -1)
            {
                std::cerr << "Problem with client connecting!" << std::endl;
                continue;
            }

            // Add the new client to the poll list
            fds.push_back({clientSocket, POLLIN, 0});

            // Clear buffers
            memset(host, 0, NI_MAXHOST);
            memset(svc, 0, NI_MAXSERV);

            int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
            if (result)
            {
                std::cout << host << " connected on " << svc << std::endl;
            }
            else
            {
                inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
            }
        }

        // Check each client socket for incoming data
        for (size_t i = 1; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buf[4096];
                memset(buf, 0, sizeof(buf));

                int bytesRecv = recv(fds[i].fd, buf, sizeof(buf), 0);
                if (bytesRecv == -1)
                {
                    std::cerr << "There was a connection issue with a client" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i; // Adjust the index since we removed a client
                    continue;
                }
                if (bytesRecv == 0)
                {
                    std::cout << "A client disconnected." << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i; // Adjust the index since we removed a client
                    continue;
                }

                // Display the received message
                std::cout << "Received from client: " << std::string(buf, 0, bytesRecv) << std::endl;

                // Echo the message back to the client
                send(fds[i].fd, buf, bytesRecv, 0);
            }
        }
    }

    // Close all sockets when done
    close(listening);
    for (auto &fd : fds)
    {
        close(fd.fd);
    }

    return 0;
}

