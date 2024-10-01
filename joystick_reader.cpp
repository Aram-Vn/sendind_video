#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <map>
#include <string>
#include <unistd.h>

// Gimbal mapping for left and right sticks
std::map<int, std::string> gimbal_map = {
    { 2, "Right Gimbal Throttle" }, // Axis 0
    { 3, "Right Gimbal Yaw" },      // Axis 1
    { 1, "Left Gimbal Pitch" },     // Axis 2
    { 0, "Left Gimbal Roll" },      // Axis 3
    { 4, "SE 3" },
    { 6, "SB 3" },
    { 5, "SF 3" },
};

void print_event(struct input_event& ev)
{
    if (ev.type == EV_ABS)
    {
        if (gimbal_map.find(ev.code) != gimbal_map.end())
        {
            std::cout << gimbal_map[ev.code] << " movement: " << ev.value << std::endl;
        }
        else
        {
            std::cout << "Axis movement (code " << ev.code << "): " << ev.value << std::endl;
        }
    }
}

int main()
{
    int fd = open("/dev/input/event21", O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "Failed to open device: " << strerror(errno) << std::endl;
        return 1;
    }

    struct libevdev* dev = nullptr;
    int              rc  = libevdev_new_from_fd(fd, &dev);
    if (rc < 0)
    {
        std::cerr << "Failed to init libevdev: " << strerror(-rc) << std::endl;
        return 1;
    }

    std::cout << "Input device name: " << libevdev_get_name(dev) << std::endl;

    while (true)
    {
        struct input_event ev;
        int                rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS)
        {
            print_event(ev);
        }
        usleep(10000); // Sleep for 10ms to reduce CPU usage
    }

    libevdev_free(dev);
    close(fd);
    return 0;
}
