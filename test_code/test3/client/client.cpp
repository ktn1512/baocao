#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define SOCKET_PATH "/tmp/user_info.sock"

struct UserInfo {
  char name[50];
  int age;
  char gender[10];
};

// Đọc đủ toàn bộ dữ liệu
bool readAll(int socket_fd, void *data, size_t size) {
  char *buffer = static_cast<char *>(data);
  size_t total_received = 0;

  while (total_received < size) {
    ssize_t received =
        read(socket_fd, buffer + total_received, size - total_received);

    if (received <= 0) {
      return false;
    }

    total_received += static_cast<size_t>(received);
  }

  return true;
}

int main() {
  // 1. Tạo UNIX Domain Socket
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("socket");
    return 1;
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;

  strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

  // 2. Kết nối đến server qua đường dẫn file
  if (connect(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
      0) {
    perror("connect");
    std::cerr << "Không thể kết nối. Server đã chạy chưa?\n";
    close(socket_fd);
    return 1;
  }

  std::cout << "[CLIENT] Đã kết nối đến server.\n";

  // 3. Nhận thông tin từ server
  UserInfo info{};

  if (!readAll(socket_fd, &info, sizeof(info))) {
    std::cerr << "[CLIENT] Không nhận đủ dữ liệu từ server.\n";
    close(socket_fd);
    return 1;
  }

  // 4. In thông tin
  std::cout << "\nThông tin nhận được từ server:\n";
  std::cout << "Họ tên   : " << info.name << '\n';
  std::cout << "Tuổi     : " << info.age << '\n';
  std::cout << "Giới tính: " << info.gender << '\n';

  close(socket_fd);

  return 0;
}