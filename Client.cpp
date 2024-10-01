#include <arpa/inet.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <unistd.h>

#define PORT        1234
#define BUFFER_SIZE 65536

int main()
{
    int                sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }

    cv::Mat            frame;
    std::vector<uchar> buffer;

    while (true)
    {
        cap >> frame;

        if (frame.empty())
        {
            std::cerr << "Empty frame, ending transmission" << std::endl;
            break;
        }

        cv::imencode(".jpg", frame, buffer);

        int bytes_sent = send(sock, buffer.data(), buffer.size(), 0);
        if (bytes_sent <= 0)
        {
            std::cerr << "Error sending data" << std::endl;
            break;
        }

        usleep(30000);
    }

    close(sock);
    cap.release();

    return 0;
}
