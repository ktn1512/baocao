#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#define SOCKET_PATH "/tmp/user_info.sock"

struct UserInfo {
  char name[50];
  int age;
  char gender[10];
};

// Gửi đủ toàn bộ dữ liệu
bool sendAll(int socket_fd, const void *data, size_t size) {
  const char *buffer = static_cast<const char *>(data);
  size_t total_sent = 0;

  while (total_sent < size) {
    ssize_t sent = send(socket_fd, buffer + total_sent, size - total_sent, 0);

    if (sent <= 0) {
      return false;
    }

    total_sent += static_cast<size_t>(sent);
  }

  return true;
}

int main() {
  // Xóa file socket cũ nếu còn tồn tại
  unlink(SOCKET_PATH);

  // 1. Tạo UNIX Domain Socket
  int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;

  strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

  // 2. Gắn socket với đường dẫn file
  if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    perror("bind");
    close(server_fd);
    return 1;
  }

  // 3. Chờ client kết nối
  if (listen(server_fd, 1) < 0) {
    perror("listen");
    close(server_fd);
    unlink(SOCKET_PATH);
    return 1;
  }

  std::cout << "[SERVER] Đang chờ client kết nối tại: " << SOCKET_PATH << '\n';

  int client_fd = accept(server_fd, nullptr, nullptr);

  if (client_fd < 0) {
    perror("accept");
    close(server_fd);
    unlink(SOCKET_PATH);
    return 1;
  }

  std::cout << "[SERVER] Client đã kết nối.\n";

  // 4. Nhập dữ liệu từ bàn phím
  std::string name;
  std::string gender;

  std::cout << "Nhập họ tên: ";
  std::getline(std::cin, name);

  std::cout << "Nhập tuổi: ";
  int age;
  std::cin >> age;

  // Xóa ký tự '\n' còn lại sau khi nhập tuổi
  std::cin.ignore();

  std::cout << "Nhập giới tính: ";
  std::getline(std::cin, gender);

  UserInfo info{};

  strncpy(info.name, name.c_str(), sizeof(info.name) - 1);
  info.age = age;
  strncpy(info.gender, gender.c_str(), sizeof(info.gender) - 1);

  // 5. Gửi dữ liệu cho client
  if (!sendAll(client_fd, &info, sizeof(info))) {
    std::cerr << "[SERVER] Gửi dữ liệu thất bại.\n";
  } else {
    std::cout << "[SERVER] Đã gửi thông tin cho client.\n";
  }

  // 6. Đóng socket và dọn dẹp
  close(client_fd);
  close(server_fd);
  unlink(SOCKET_PATH);

  return 0;
}