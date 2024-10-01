#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <map>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// Gimbal mapping for left and right sticks
std::map<int, std::string> gimbal_map = {
    { 2, "Right Gimbal Throttle" }, // Axis 0
    { 3, "Right Gimbal Yaw" },      // Axis 1
    { 1, "Left Gimbal Pitch" },     // Axis 5
    { 0, "Left Gimbal Roll" },      // Axis 3
    { 4, "SE 3" },
    { 6, "SB 3" },
    { 5, "SF 3" },
};

void print_event(struct input_event& ev, int sockfd)
{
    std::string message;
    if (ev.type == EV_ABS)
    {
        if (gimbal_map.find(ev.code) != gimbal_map.end())
        {
            message = gimbal_map[ev.code] + " movement: " + std::to_string(ev.value);
        }
        else
        {
            message = "Axis movement (code " + std::to_string(ev.code) + "): " + std::to_string(ev.value);
        }
        std::cout << message << std::endl;

        // Send the message over the TCP socket
        send(sockfd, message.c_str(), message.size(), 0);
    }
}

int main()
{
    // Set up TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return 1;
    }

    // Server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(8080); // Port number
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    int fd = open("/dev/input/event21", O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "Failed to open device: " << strerror(errno) << std::endl;
        close(sockfd);
        return 1;
    }

    struct libevdev* dev = nullptr;
    int              rc  = libevdev_new_from_fd(fd, &dev);
    if (rc < 0)
    {
        std::cerr << "Failed to init libevdev: " << strerror(-rc) << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Input device name: " << libevdev_get_name(dev) << std::endl;

    while (true)
    {
        struct input_event ev;
        int                rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS)
        {
            print_event(ev, sockfd);
        }
    }

    libevdev_free(dev);
    close(fd);
    close(sockfd);
    return 0;
}
