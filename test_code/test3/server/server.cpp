#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

// Cấu trúc dữ liệu dùng để truyền IPC
struct UserInfo {
    char name[50];
    int age;
    char gender[10];
};

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 1. Tạo Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Tạo Socket thất bại!\n";
        return -1;
    }

    // Cho phép tái sử dụng địa chỉ/port ngay lập tức
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 2. Bind Socket vào IP/Port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind thất bại!\n";
        return -1;
    }

    // 3. Lắng nghe kết nối từ Client
    if (listen(server_fd, 1) < 0) {
        std::cerr << "Listen thất bại!\n";
        return -1;
    }

    std::cout << "[SERVER] Đang chờ Client kết nối tới Port " << PORT << "...\n";

    // 4. Chấp nhận kết nối từ Client
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept thất bại!\n";
        return -1;
    }

    std::cout << "[SERVER] Client đã kết nối thành công!\n\n";

    // 5. Nhập thông tin từ bàn phím tại Server
    UserInfo info;
    std::cout << "=== NHẬP THÔNG TIN TẠI SERVER ===\n";
    std::cout << "Nhập họ tên: ";
    std::cin.getline(info.name, sizeof(info.name));
    
    std::cout << "Nhập tuổi: ";
    std::cin >> info.age;
    std::cin.ignore(); // Xóa bộ đệm bàn phím

    std::cout << "Nhập giới tính: ";
    std::cin.getline(info.gender, sizeof(info.gender));

    // 6. Gửi cấu trúc dữ liệu sang Client qua IPC
    send(new_socket, &info, sizeof(info), 0);
    std::cout << "\n[SERVER] Đã gửi thông tin sang Client qua IPC thành công!\n";

    // 7. Đóng Socket
    close(new_socket);
    close(server_fd);

    return 0;
}