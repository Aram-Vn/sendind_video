#include <arpa/inet.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1234
#define BUFFER_SIZE 65536

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    std::cerr << "Socket creation error" << std::endl;
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Bind failed" << std::endl;
    return -1;
  }

  if (listen(server_fd, 3) < 0) {
    std::cerr << "Listen failed" << std::endl;
    return -1;
  }

  std::cout << "Waiting for a connection on port " << PORT << "..."
            << std::endl;

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    std::cerr << "Accept failed" << std::endl;
    return -1;
  }

  std::cout << "Connection established with client." << std::endl;

  cv::Mat frame;
  std::vector<uchar> buffer(BUFFER_SIZE);

  while (true) {
    int bytes_received = recv(new_socket, buffer.data(), buffer.size(), 0);
    if (bytes_received <= 0) {
      std::cerr << "Connection closed or error receiving data." << std::endl;
      break;
    }

    std::vector<uchar> packet(buffer.begin(), buffer.begin() + bytes_received);
    frame = cv::imdecode(packet, cv::IMREAD_COLOR);

    if (frame.empty()) {
      std::cerr << "Error decoding frame" << std::endl;
      continue;
    }

    cv::imshow("Server Video Stream", frame);

    if (cv::waitKey(30) >= 0) {
      break;
    }
  }

  // Step 6: Cleanup
  close(new_socket);
  close(server_fd);
  cv::destroyAllWindows();

  return 0;
}
