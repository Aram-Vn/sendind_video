#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <unistd.h>

int main()
{
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return 1;
    }

    // Set up the server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;

    // Bind to the specific IP address of the Raspberry Pi
    if (inet_pton(AF_INET, "192.168.27.20", &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid IP address: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    server_addr.sin_port = htons(8080); // Port number

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0)
    {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Waiting for connections on 192.168.27.20:8080..." << std::endl;

    // Accept a connection
    sockaddr_in client_addr;
    socklen_t   client_len = sizeof(client_addr);
    int         newsockfd  = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (newsockfd < 0)
    {
        std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // Buffer to store received messages
    char buffer[256];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        ssize_t bytes_received = recv(newsockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0)
        {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break;
        }
        else if (bytes_received == 0)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        // Print the received message
        std::cout << "Received: " << buffer << std::endl;
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
