#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

int main()
{
    const char* portname = "/dev/ttyACM0";
    int         fd       = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        std::cerr << "Error opening port" << std::endl;
        return -1;
    }

    termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B100000); // Set baud rate
    cfsetospeed(&options, B100000);
    options.c_cflag |= (CLOCAL | CREAD);                // Enable receiver, ignore modem control lines
    options.c_cflag &= ~PARENB;                         // No parity
    options.c_cflag &= ~CSTOPB;                         // 1 stop bit
    options.c_cflag &= ~CSIZE;                          // Clear the current char size mask
    options.c_cflag |= CS8;                             // 8 data bits
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    tcsetattr(fd, TCSANOW, &options);

    while (true)
    {
        char buffer[256];
        int  n = read(fd, buffer, sizeof(buffer));
        if (n > 0)
        {
            std::cout.write(buffer, n);
            std::cout.flush();
        }
    }

    close(fd);
    return 0;
}
