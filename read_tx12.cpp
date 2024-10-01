#include <boost/asio.hpp>
#include <iostream>
#include <vector>

int main()
{
    try
    {
        // Common baud rates to test
        std::vector<int> baud_rates = { 9600, 19200, 38400, 57600, 115200, 230400, 460800 };

        // Set up the IO service
        boost::asio::io_service io;

        for (int baud_rate : baud_rates)
        {
            std::cout << "Trying baud rate: " << baud_rate << std::endl;

            try
            {
                // Open the serial port
                boost::asio::serial_port serial(io, "/dev/ttyACM0");

                // Configure serial port parameters
                serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
                serial.set_option(boost::asio::serial_port_base::character_size(8));
                serial.set_option(
                    boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
                serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
                serial.set_option(
                    boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

                // Set a timeout (e.g., 2 seconds) for reading data
                boost::asio::deadline_timer timer(io);
                timer.expires_from_now(boost::posix_time::seconds(2));

                char                      data[128];
                boost::system::error_code ec;
                size_t                    len = 0;

                // Use a lambda function to handle the timeout
                timer.async_wait(
                    [&serial](const boost::system::error_code& ec)
                    {
                        if (serial.is_open())
                        {
                            std::cerr << "Read operation timed out.\n";
                            serial.cancel();
                        }
                    });

                // Reading data (non-blocking)
                serial.async_read_some(boost::asio::buffer(data),
                                       [&](const boost::system::error_code& err, std::size_t bytes_transferred)
                                       {
                                           ec  = err;
                                           len = bytes_transferred;
                                       });

                // Run the IO service to execute the asynchronous operations
                io.run();
                io.reset(); // Reset IO service for the next iteration

                if (ec == boost::asio::error::eof)
                {
                    std::cerr << "Connection closed by transmitter at baud rate " << baud_rate << std::endl;
                    break;
                }
                else if (ec)
                {
                    throw boost::system::system_error(ec); // Other errors
                }

                // If we get data, print it and stop the scan
                if (len > 0)
                {
                    std::cout << "Data received at baud rate " << baud_rate << ":\n";
                    std::cout.write(data, len); // Print received data to console
                    std::cout << std::endl;
                    break; // Exit the loop if we successfully read data
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error at baud rate " << baud_rate << ": " << e.what() << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }

    return 0;
}
