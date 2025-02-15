#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main() {
    // Create the socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Can't create socket!" << std::endl;
        return -1;
    }

    // Set up the server address
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);  // Port where the server is listening
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);  // Assuming the server is running on localhost

    // Connect to the server
    if (connect(clientSocket, (sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "Can't connect to the server!" << std::endl;
        close(clientSocket);
        return -2;
    }

    std::cout << "Connected to server!" << std::endl;

    // Send a message to the server
    std::string message;
    char buf[4096];

    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;  // Exit the loop if the user types "exit"
        }

        // Send the message to the server
        send(clientSocket, message.c_str(), message.length(), 0);

        // Clear the buffer
        memset(buf, 0, sizeof(buf));

        // Receive the echoed message from the server
        int bytesRecv = recv(clientSocket, buf, sizeof(buf), 0);
        if (bytesRecv == -1) {
            std::cerr << "Error receiving message from server!" << std::endl;
            break;
        } else if (bytesRecv == 0) {
            std::cout << "Server disconnected!" << std::endl;
            break;
        }

        // Display the echoed message
        std::cout << "Server echoed: " << std::string(buf, 0, bytesRecv) << std::endl;
    }

    // Close the socket
    close(clientSocket);
    std::cout << "Disconnected from server." << std::endl;

    return 0;
}
