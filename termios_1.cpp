#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;

int main()
{
    io_service  io;
    serial_port serial(io, "/dev/ttyACM0"); // Use the detected serial device

    // Set serial port parameters
    serial.set_option(serial_port_base::baud_rate(115200)); // Adjust baud rate as needed
    serial.set_option(serial_port_base::character_size(8));
    serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial.set_option(serial_port_base::parity(serial_port_base::parity::none));

    while (true)
    {
        char c;
        boost::asio::read(serial, buffer(&c, 1)); // Read one character
        std::cout << c;                           // Output the received character
    }

    return 0;
}
