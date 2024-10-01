#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <unistd.h>

int main()
{
    const char* device_path = "/dev/input/event21"; // Update this if necessary
    int         fd          = open(device_path, O_RDONLY | O_NONBLOCK);
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
        close(fd);
        return 1;
    }

    std::cout << "Input device: " << libevdev_get_name(dev) << "\n";
    std::cout << "ID: bus=0x" << std::hex << libevdev_get_id_bustype(dev) << ", vendor=0x"
              << libevdev_get_id_vendor(dev) << ", product=0x" << libevdev_get_id_product(dev) << std::dec << "\n";

    struct input_event ev; // Declare the input_event structure

    // Main loop to read events
    while (true)
    {
        int rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS)
        {
            // Handle the joystick event
            std::cout << "Event: type=" << ev.type << ", code=" << ev.code << ", value=" << ev.value << "\n";
        }
        else if (rc == LIBEVDEV_READ_STATUS_SYNC)
        {
            std::cout << "Event sync: " << strerror(rc) << "\n";
        }
    }

    // Clean up
    libevdev_free(dev);
    close(fd);
    return 0;
}
